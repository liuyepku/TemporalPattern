
/** \file matinvert.cpp
    \author Frank Hoeppner <frank.hoeppner@ieee.org>

    990218 fh: first version
*/

#ifndef FH_MATINVERT_SOURCE
#define FH_MATINVERT_SOURCE

#include "logtrace.h" // Log/Trace

template <class M>
void gauss_jordan
(
  M& A
)
{
  LOG_FUNCTION("","gauss_jordan",A);
  
  int hi,i,j,k,r,pivot[A.cols()],n = A.cols();
  typename M::value_type max,hr,hv[A.cols()];
  
  INVARIANT( n==A.rows(), "inv(AbsMatrix) (dimension mismatch)");
  
  // Gau\3-Jordan-Verfahren, Stoer S. 161
  for (j=0;j<n;++j) 
  {
    pivot[j]=j;
  }
    
  for (j=0;j<n;++j)
  {
    // Pivotsuche (bestimme Pivotelement fuer j.-te Zeile
    max = fabs(A(j,j)); 
    r = j;
    TRACE("search for pivot element "<<j<<';'<<A);
    for (i=j+1;i<n;++i)
    {
      hr = fabs(A(i,j));
      if (hr>max) 
      { 
        max=hr; 
        r=i; 
      }
    }
    TRACE("select row "<<r);
    INVARIANT(max!=0.0,"gauss_jordan (singularity)");

    // Zeilentausch
    if (r>j)
    {
      for (k=0;k<n;k++) 
      { 
        hr=A(j,k); 
        A(j,k)=A(r,k); 
        A(r,k)=hr;     
      }

      hi=pivot[j]; 
      pivot[j]=pivot[r]; 
      pivot[r]=hi;
    }


    // Transformation
    hr=1.0/A(j,j);
    for (i=0;i<n;i++) A(i,j)*=hr;
    A(j,j)=hr;
    for (k=0;k<n;k++) if (k!=j)
    {
      for (i=0;i<n;i++) if (i!=j)
        A(i,k)-=A(i,j)*A(j,k);
      A(j,k)*=-hr;
    }
  }

  // Spaltentausch
  for (i=0;i<n;i++)
  {
    for (k=0;k<n;k++) hv[pivot[k]]=A(i,k);
    for (k=0;k<n;k++) A(i,k)=hv[k];
  }
}

#endif // FH_MATINVERT_SOURCE
