


#ifndef CFD_BICGSTAB_H
#define CFD_BICGSTAB_H

SubmitObjects(){
}


template < class Operator, /*class Vector, */class DataVector, class Preconditioner, class Real >
int
BiCGSTAB(/*const */Operator &A, DataVector &x, DataVector &b,
         Preconditioner &M, int &max_iter, Real &tol)
{
  Real resid;
  //Vector rho_1(1), rho_2(1), alpha(1), beta(1), omega(1);
  Real rho_1, rho_2, alpha, beta, omega;
  DataVector p, phat, s, shat, t, v;

  Real normb = Norm(b);
  //DataVector r = b - A * x;
  DataVector r;
  A.MultAdd(x,b,r,-1.,1.);
  DataVector rtilde = r;

  if (normb == 0.0)
    normb = 1;

  if ((resid = Norm(r) / normb) <= tol) {
    tol = resid;
    max_iter = 0;
    return 0;
  }

  for (int i = 1; i <= max_iter; i++) {
    //rho_1(0) = dot(rtilde, r);
    rho_1 = Dot(rtilde, r);
    //if (rho_1(0) == 0) {
    if (rho_1 == 0) {
      tol = Norm(r) / normb;
      return 2;
    }
    if (i == 1)
      p = r;
    else {
      //beta(0) = (rho_1(0)/rho_2(0)) * (alpha(0)/omega(0));
      beta = (rho_1/rho_2) * (alpha/omega); //TComputeBeta
      //p = r + beta(0) * (p - omega(0) * v);
      p *= beta; //p.Add(beta,1)
      p.Add(1., r);
      p.Add(- beta * omega, v);
    }
    M.Solve(p, phat);//parei aqui.
    //v = A * phat;
    A.Multiply(phat, v);

    //alpha(0) = rho_1(0) / dot(rtilde, v);
    alpha = rho_1 / Dot(rtilde, v);
    //s = r - alpha(0) * v;
    s = r;
    s.Add(- alpha, v);
    
    if ((resid = Norm(s)/normb) < tol) {
      //x += alpha(0) * phat;
      x.Add(alpha, phat);
      tol = resid;
      return 0;
    }
    M.Solve(s, shat);
    //t = A * shat;
    A.Multiply(shat, t);
    omega = Dot(t,s) / Dot(t,t);
    //x += alpha(0) * phat + omega(0) * shat;
    x.Add(alpha, phat);
    x.Add(omega, shat);
    //r = s - omega(0) * t;
    r = s;
    r.Add(- omega, t);

    //rho_2(0) = rho_1(0);
    rho_2 = rho_1;
    if ((resid = Norm(r) / normb) < tol) {
      tol = resid;
      max_iter = i;
      return 0;
    }
    //if (omega(0) == 0) {
    if (omega == 0) {
      tol = Norm(r) / normb;
      return 3;
    }
  }

  tol = resid;
  return 1;
}

