#ifndef TPZFPARMATRIXH
#define TPZFPARMATRIXH
#include "pzfmatrix.h"

#include "OOPDataManager.h"
#include "OOPTaskManager.h"
#include "OOPWaitTask.h"


#define TPZFPARZAXPYTASK_ID 33101
#define TPZFPARDOTTASK_ID		33102
#define TPZFPARMATRIX_ID		33103


extern OOPDataManager * DM;

/**
 * @brief Implements a parallel full matrix for the OOPar environment.
 */
/**
 * Must support the required interfaces for some specific parallel operations.
 * The CG defines the necessary interface the the class must provide.
 * The CG necessary interfaces are implemented in the TPZParDohrMatrix and whenever a FullMatrix is required
 * a TPZFParMatrix will be used instead. Note that TPZFParMatrix is not the class wich is calling the CG method,
 * the TPZFParMatrix is actually used by the TPZParDohrMarix which calls the CG method.
 * Understanding the details of the CG Template implementation are key to understanding the TPZFParMatrix required
 * reimplementation, which will be implemented in parallel using OOPar as parallel environment.
 */
class TPZFParMatrix : public TPZFMatrix {
public:
	/**
	 * @brief Simple constructor
	 */
	TPZFParMatrix () : TPZFMatrix()
	{
		TPZFMatrix * matrix = new TPZFMatrix;
		m_Id = DM->SubmitObject(matrix);
		m_IsSync = false;
	}
	
	inline TPZFParMatrix(TPZFMatrix & matrix) : TPZFMatrix(matrix)
	{
		TPZFMatrix * lMatrix = new TPZFMatrix(matrix);
		m_Id = DM->SubmitObject(lMatrix);
		m_IsSync = false;
		
	}
    
	/**
	 * @brief Read and Write methods
	 */
	virtual void Read(TPZStream &buf, void *context);
	virtual void Write(TPZStream &buf, int withclassid);
	/**
	 @brief Constructor with initialization parameters
	 @param rows Initial number of rows
	 @param columns Number of columns
	 @param buf Preallocated memory area which can be used by the matrix object
	 @param size Size of the area pointed to by buf
	 */
	TPZFParMatrix (const int rows ,const int columns, REAL * buf,const int size)
	: TPZFMatrix(rows, columns, buf, size)
	{
		TPZFMatrix * matrix = new TPZFMatrix(rows, columns, buf, size);
		m_Id = DM->SubmitObject(matrix);
		m_IsSync = false;
	}
	/**
	 @brief Constructor with initialization parameters
	 @param rows Initial number of rows
	 @param columns Number of columns
	 @param val Inital value fill all elements
	 */
	TPZFParMatrix (const int rows ,const int columns,const REAL & val )
	: TPZFMatrix(rows, columns, val)
	{
		TPZFMatrix * matrix = new TPZFMatrix(rows, columns, val);
		m_Id = DM->SubmitObject(matrix);
		m_IsSync = false;
	}
	/**
	 @brief Constructor with initialization parameters
	 @param rows Initial number of rows
	 @param columns Number of columns
	 */
	inline TPZFParMatrix(const int rows ,const int columns = 1) : TPZFMatrix(rows,columns)
	{
		TPZFMatrix * matrix = new TPZFMatrix(rows, columns);
		m_Id = DM->SubmitObject(matrix);
		m_IsSync = false;
	}
	
	/**
	 * @brief Copy constructor
	 * @param copy Used as a model for current object
	 */
	TPZFParMatrix (const TPZFParMatrix & copy);
	/**
	 * @brief Destructor
	 */
	~TPZFParMatrix();
	
	/**
	 * @brief According to the documentation in base class. \n
	 * It is performed with call to SynchronizeToLocal() and subsequently a 
	 * call to ZAXPY of the base class.
	 */
	void ZAXPY(const REAL alpha, const TPZFParMatrix &p);
	void ZAXPY(const REAL alpha, const TPZFMatrix & p);
	
	
	void MultAdd(const TPZFParMatrix &x, const TPZFParMatrix &y, TPZFParMatrix &z, const REAL alpha = 1., const REAL beta = 0., const int opt = 0, const int stride = 1) const;
	void MultAdd(const TPZFMatrix &x, const TPZFMatrix &y, TPZFMatrix &z, const REAL alpha = 1., const REAL beta = 0., const int opt = 0, const int stride = 1) const;
	
	
	void Multiply(const TPZFParMatrix &A, TPZFParMatrix&B, int opt=0, int stride=1) const ;
	void Multiply(const TPZFMatrix &A, TPZFMatrix&B, int opt=0, int stride=1) const ;
	
	void TimesBetaPlusZ(const REAL beta, const TPZFParMatrix &z);
	void TimesBetaPlusZ(const REAL beta, const TPZFMatrix &z);
	
	OOPObjectId Id() const
	{
		return m_Id;
	}
	OOPDataVersion Version() const
	{
		return m_Version;
	}
	void IncrementVersion()
	{
		m_Version++;
	}
	OOPDataVersion GetNextVersion()
	{
		OOPDataVersion ver(m_Version);
		ver++;
		return ver;
	}
	OOPDataVersion GetVersionAfterWrites(int nWriteAccess)
	{
		OOPDataVersion ver(m_Version);
		for(int i = 0; i < nWriteAccess; i++)
		{
			ver.Increment();
		}
		return ver;
	}
	
	/**
	 * @brief Synchronizes distributed data to local data. \n
	 * A call to SynchronizeToLocal increments the version of the distributed object.
	 * @note After a call to this method both distributed and local copies are identical
	 */
	void SynchronizeFromRemote();
	void SynchronizeFromLocal();
	
	
	int ClassId()
	{
		return TPZFPARMATRIX_ID;
	}
	
	int Zero();
	
	int Redim(const int rows, const int cols);
	
	
	virtual void SolveCG(int & 	numiterations, TPZSolver & 	preconditioner, const TPZFMatrix & 	F, TPZFMatrix & result, TPZFMatrix * residual, REAL & tol, const int 	FromCurrent = 0);
	
	virtual TPZFMatrix & operator = (const TPZFParMatrix & copy);
	virtual TPZFMatrix & operator = (const TPZFMatrix & copy);
	
	
protected:
private:
	/**
	 * @brief Holds the Object Id of the distributed part of the object
	 */
	OOPObjectId m_Id;
	/**
	 * @brief Holds the version of the distributed part of the object.
	 * @note Important : This version tracks the actual version of the object on the parallel environment.
	 * It is not an axact copy of that distributed version.
	 * When a call to a distributed method is performed, version arithmetic is performed on that Version object. It is used as the tracking version of the distributed part
	 */
	OOPDataVersion m_Version;
	
	/**
	 * @brief Indicates if the distributed and local copy are the same. \n
	 * Some operations which are going to be called locally requires such state.
	 */
	bool m_IsSync;
	
public:
	
	class TPZAccessParMatrix
	{
	public:
		/**
		 * @brief Access to distributed object with Read permission
		 */
		TPZAccessParMatrix(const TPZFParMatrix & par);
		/**
		 * @brief Access to distributed object with Write access
		 */
		TPZAccessParMatrix(TPZFParMatrix & par);
		
		~TPZAccessParMatrix();
		
		TPZFMatrix & GetMatrix();
	private:
		OOPWaitTask * m_WT;
		
	};
	
};


REAL Dot(const TPZFParMatrix &A,const TPZFParMatrix &B);

REAL Norm(const TPZFParMatrix &A);

#endif
