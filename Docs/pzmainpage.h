/**
\mainpage The OOPar environment

\author Philippe Remy Bernard Devloo <a href="http://lattes.cnpq.br/6051486998967925">Lattes</a>
\author Jorge Lizardo Diaz Calle <a href="http://lattes.cnpq.br/2049910703027682">Lattes</a>
\author Edimar Cesar Rylo <a href="http://lattes.cnpq.br/7462096912445959">Lattes</a>
\author Gustavo Camargo Longhin <a href="http://lattes.cnpq.br/9121612523149859">Lattes</a>
\author Mauro Enrique de Souza Munoz

The OOPar environment is a object oriented environment for the development parallel softwares using finite element method.

Parallel programming increases the dimension of complexity of the program: the parallel programming paradigm requires the use of new concepts such as synchronism and barriers, and if asynchronous communication is used, the execution path of the program becomes non-deterministic. Several object oriented interfaces exist for parallel programming.

The object oriented interface to parallel programming (OOPar) presented develops a new paradigm which offers an interface to the message passing interface library, a distributed data model and a parallel task concept. Its intention is to be coupled to the NeoPZ environments, to allow NeoPZ to be executed on parallel computers.

Using structured programming, the author has developed several programs for parallel finite element computing, and has noticed the following bottlenecks:

1. The impact of the message passing interface on the code structure is very great. If data structures of any level of complexity need to be shared between processors, sending and receiving routines become very complex.

2. It is virtually impossible to escape from synchronised message passing. When a process sends a message to another process, the receiving process needs to know beforehand which data structure will be received.

3. Parallel programs are of the branch and join type, with obvious synchronisation barriers and sequential code bottlenecks. Coding complexity inhibits any other approach.

\section sec_layers Abstraction layers implemented in OOPar.

This section describes and motivates several abstraction layers which are implemented in OOPar. These layers should be sufficient to write asynchronous parallel programs, transmitting complex data structures between processes. The different abstraction layers are:

1. Abstraction of message passing. Communications manager. \ref sec_absmpi "Abstraction MPI" \n
2. Abstraction of the distributed pointer mechanism. \ref sec_absdpm "Abstraction mechanism" \n
3. Abstraction of distributed objects. Data manager. \ref sec_datamanager "Data manager" \n
4. Tasks. \ref sec_task "Task" \n
5. Administration of tasks. Task manager. \ref sec_taskmanager "Task manager" \n

\page description Describing abstractions

\section sec_absmpi Abstraction of the Message Passing Interface: Communication manager.

MPI allows only simple data structures to be transmitted between processes. Each send or receive procedure corresponds to a single data item. When working with object oriented programming languages, data structures tend to become increasingly complex. MPI allows the user to pack objects of different type within a buffer which is transmitted as a single message. The packing of data structures as a stream of bytes and subsequent object recomposing is often referred to as object serialisation. Within OOPAR, serialised classes are derived from the base class TPZSaveable.

The TPZSaveable class defines the interface which derived classes need to implement. This interface is virtual void Write(TSendbuffer *); which will pack the data contained within the object
to the send buffer
virtual void Read(TReceivebuffer *bf,
int &position);
which converts the stream of bytes into the original
object. Note that the Write and Read may be
called recursively. To initiate the object recombination
based on a stream of bytes, a class recombination
method is associated with each serialised
class. The first integer of transmitted method identifies
the procedure needed to recompose the object.
The interface of the procedure is
TPZSaveable *Restore (TReceiveBuffer *bf,
int &position);
A pointer to this function can be found by calling
GetRestore:
typedef TPZSaveable *(*) (TReceiveBuffer *bf,
int &position) TRestoreFunction;
TRestoreFunction GetRestore (long classid);
GetRestore returns a pointer to the procedure which will recompose an object of a type identified
by an ID.
Using the above described procedure, it is possible
to transmit objects of arbitrary types between processes.
The problem with which the programmer is
now faced is: What is the destination of the received
objects? When a process receives an object of a
given type, what does it need to do with it? After
an object has been transmitted to another process,
how can the original process refer to the transmitted
object. These problems are addressed in the next
abstraction layer.

\section sec_absdpm Abstraction of the Distributed Pointe Mechanism

Within a sequential or multi-threaded environment,
object are identified by their address. Within a parallel
computer with distributed memory, the pointer
mechanism cannot be used anymore to identify the
objects. To overcome this problem, the OOPAR
introduces the distributed data object, which assigns
a unique identity with each object which needs to
be shared among processes. With this mechanism,
the user of OOPAR can associate a serialised object
with a serialised object, transmit the distributed data
object to another process and use it within a parallel
algorithm. The distributed data objects are managed
by the Data Manager, which stores them in a binary
tree for rapid access. A version number with each
distributed data object: this allows to associated a
state with each object.

The next pending problem is: there is a distributed
data model, but the operations/algorithms which will
be performed on this distributed data must still be
hard coded. This means that each process must
know what type of data objects will be transmitted,
and which operations need to be performed on
these. This model still doesn’t allow to write general
purpose parallel software.

\section sec_task Tasks

A task is an object which, through its virtual execute
method, will access/transform distributed data
objects. Each task object belongs to a class derived
from TTask which in turn is derived from TPZSaveable.
Therefore, task objects can be transmitted from
one process to another.
In order to execute, task objects depend upon
accessing distributed data objects. Two types of
access are distinguished: read access and write
access. The task object is not supposed to modify a distributed data object to which it has read access;
it will generally modify distributed data objects to
which it has write access. The task which modifies
a distributed data object will generally increase its
version number.
The execution sequence of a parallel program
now consists in creating task objects of the appropriate
type, and giving the appropriate data dependency
to each task object. There are still two elements
missing:
1. As tasks depend upon data access, it is conceivable that two task objects will want to access a distributed data object simultaneously. Distributed data objects exist, but there is no arbitration scheme which will decide which task object has access to which data. This arbitration will be implemented within the Data Manager.
2. Several tasks can be created simultaneously. Some tasks can be set up for execution, while others need to be queued till their data dependency is satisfied. The set of tasks is administered by the Task Manager.

\section sec_datamanager Administering distributed Data objects: Data manager

The Data Manager assigns a unique identity to
serialised objects. After a serialised object has been
submitted to the Data Manager, the Data Manager
will administer all access to that object. One process
is considered as the owner of a distributed data
object. Data access at other processes is granted at
the process level. Many processes can access a
distribute data object simultaneously, but only one
process can have write access to a distributed data
object. The process which has write access to the
object is considered owner of the object. Requests
for data access are queued at each object. If a data
access requested by a task can be satisfied, the Data
Manager notifies the task object and passes it the
pointer to the requested object. After a task terminates,
it notifies the Data Manager it doesn’t need
the data access anymore. The Data Manager can
then grant access to another task or process which
queued a request.

\section sec_taskmanager Administering task objects: Task manager

The execution sequence of a parallel program within
OOPAR generally follows the execution sequence
of the data flow paradigm: a set of tasks are queued for execution, each one depending on a particular
version of a distributed data object. As the object
is gradually modified by the execution of the tasks,
the parallel algorithm is executed. Tasks are queued
within the Task Manager. The Task Manager verifies
whether an queued task has received all data
accesses it requested and, if affirmative, sets the
task up for execution. Tasks execute in separate
threads, which implies that data communication is
done separately from the execution of the parallel
program. This also implies that several tasks can be
executed in parallel within a single process, which
makes OOPAR suitable for shared memory
machines as well.
Within OOPAR, the user does not have to worry
about the explicit transfer of objects: objects are
transmitted between processes in response to access
requests of tasks or other processes. Tasks can
execute on any process, which implies that OOPAR
is an appropriate environment to experiment with
dynamic load balancing strategies.

\section sec_conclusions Opinion

Within the effort of the author to develop object
oriented scientific software, three different environments
have been distinguished: an environment for
linear algebra, an environment for the development
of finite element software and an environment for
developing parallel software. The development
philosophy of each environment is considered more
important that the environments itself. As such, the
matrix environment is extendible to include new
matrix storage forms, the finite element environment
separates the geometric map from the definition of
the interpolation space, and from the definition of
the system of partial differential equations and the
parallel environment implements the concept of distributed
data objects and tasks.

\section sec_doc_structure Structure of the Documentation

There are many ways to define a library of classes. A global view of the OOPar environment is
found in \ref page_structure. This same structure is "more or less" recognized in the 
<a href="modules.html">Modules</a> section.




\page page_structure Structure of the OOPar Environment

\page adv_technologies Parallel Programming Technologies

\section sec_advancempi Message Passing Interface

\section sec_advancepvm Parallel Virtual Machine
 

 \page utilitaries Utilitaries to easy compile of the NeoPZ environment
 
 \section svn Getting OOPar code
 
 It is recommended to use <a href="http://www.syntevo.com/smartsvn/download.html?all=true">SmartSVN</a> to get the OOPar code. Actually we are using SmartSVN 6.6.9 .
 
 \section cmake Creating project depending on the system user
 
 It is recommended to use <a href="http://www.cmake.org/cmake/resources/software.html">CMake</a> to generate the oopar library depending on your system. \n
 Actually we are using CMake 2.8.5 .
 
 \section doxygen Generating documentation
 
 It is recommended to use <a href="http://www.stack.nl/~dimitri/doxygen/download.html#latestsrc">Doxygen</a> to generate the oopar documentation. \n
 Actually we are using Doxygen 1.7.5.1 . To right compiling using doxygen you must to have the following executables:
 
 \li <a href="http://www.cs.utah.edu/dept/old/texinfo/dvips/dvips.html">dvips</a> or 
 <a href="http://rpmfind.net/linux/rpm2html/search.php?query=ghostscript-dvipdf">dvipdf</a> - Convert tex to ps (post script) or pdf format.

 \section manuals Manuals
 
 To get or access the manuals clik on following links:
 \li <a href="http://www.syntevo.com/download/smartsvn/smartsvn-reference.pdf">SmartSVN</a>
\li <a href="http://www.cmake.org/cmake/help/cmake-2-8-docs.html">CMake</a> 
 \li <a href="ftp://ftp.stack.nl/pub/users/dimitri/doxygen_manual-1.7.5.1.pdf.zip">Doxygen</a> 
 
\page externlibs External Libraries used in OOPar

It is recommended to create a directory libs or externallibs at the same level as oopar project. \n
External libraries which are not necessarily installed at root lib directory or root include directory \n
can to be installed or copied into the indicated directory.

OOPar uses until five external libraries:
 \li \ref pz "pz" - This is not optional. To implements simulations in finite element environment.
 \li \ref pthread "pthread" - For the POSIX pthread threading.
 \li \ref boost "boost" - It provides free portable peer-reviewed C++ libraries. We mainly was used as unit test framework.
 \li \ref log4cxx "log4cxx" - For efficient log messages.
 \li \ref metis "metis" - To partitioning finite element meshes, and producing fill reducing orderings for sparse matrices.
 
 \section pz NeoPZ library
 
 To get the source code, use this command to anonymously check out the latest project (in terminal):
 svn checkout http://neopz.googlecode.com/svn/trunk/ neopz-read-only
 Or use the SVN program:
 . In the first windows check: Check out project from repository
 . In the second windows:
 - Check Quick Checkout (less configuration)
 - Write http://neopz.googlecode.com/svn/trunk/ in URL:
 - Choose your local directory where you want to store neopz source code.
 
 To install pz library (libpz.a), 
 \li Run CMake using the CMakeList.txt in neopz root directory.
 \li Run PZ.xcodeproj into the neopz_CMake directory
 \li Execute the target install. It's ready.
 
 Then, into the externallibs directory will be installed the libpz.a and the header files into the include directory.
 
 \section boost Boost library
 If you set USING_BOOST as TRUE it is necessary to install the Boost library. \n
 Get the latest version of BOOST library at download from <a href="http://sourceforge.net/projects/boost/files/boost">Sourceforge</a>. \n
 
 It is recommended to use version <a href="http://sourceforge.net/projects/boost/files/boost/1.47.0">1.47.0</a>
 
 Use boost_1_47_0.tar.gz or boost_1_47_0.tar.bz2 for unix or mac systems. \n
 Use boost_1_47_0.7z or boost_1_47_0.zip for windows system.
 
 To install following next steps:\n
 Uncompress the version downloaded. \n
 Using command line change into the uncompress directory. \n
 For mac or unix systems, type the following commands and execute: \n
 \li sudo ./bootstrap.sh
 \li sudo ./bjam install
 
 For Windows systems, execute: \n
 \li ./bootstrap.bat
 \li ./bjam install
 
 See <a href="http://www.boost.org/doc/libs/1_47_0/doc/html/bbv2/installation.html">Installation</a>
 
 \section log4cxx Log4cxx library
 
 Apache <a href="http://logging.apache.org/log4cxx/">log4cxx</a> is a logging framework for C++ pattern. It has three main components: loggers, appenders and layouts.
 These three types of components work together to enable developers to log messages according to message type and level, 
 and to control at runtime how these messages are formatted and where they are reported.
 
 To install, <a href="http://logging.apache.org/log4cxx/download.html">download</a> apache-log4cxx-0.10.0.zip for windows system, or apache-log4cxx-0.10.0.tar.gz for unix or mac systems. \n
 Then uncompress the archive. Using command line change into uncompress directory. \n
 Type de following commands: \n
 \li ./configure
 \li make check
 \li sudo make install
 
 \section metis Metis library
If you set USING_METIS as TRUE, you must to install Metis library. \n
Metis library is avaliable 
from <a href="http://glaros.dtc.umn.edu/gkhome/metis/metis/download">Karypis Lab</a>

METIS is a set of serial programs for partitioning graphs, partitioning finite element meshes, \n
and producing fill reducing orderings for sparse matrices.
 
\section pthread Pthread library
 
The oopar project uses <a href="http://staff.science.uva.nl/~bterwijn/Projects/PThread/">PThread</a> library for the POSIX pthread threading. 
To install for unix or mac systems, make <a href="http://staff.science.uva.nl/~bterwijn/Projects/PThread/PThread.tar.gz">download</a> of the source code. Uncompress the archive and using command line change into the uncompress directory with PThread.
Use the following commands:
 \li ./configure
 \li make
 \li sudo make install
 
 For windows system, get the header files and release libraries (dll, lib) from <a href="http://sourceware.org/pthreads-win32/>sourceware</a>. You can to connect with server as guest and then copy the include and lib directories into your system.
 
\page tutorial Tutorial examples

\section use_substruct Sub structure
 
\section use_parsubstruct Parallel sub structure

\section use_task Testing Task
 
\page projects Projects with NeoPZ
 
\section prj_flux Flux
 
\section prj_cesar Cesar Test
  
*/
