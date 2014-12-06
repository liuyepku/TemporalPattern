

template <class T>
class GridTable
  : public T
  {
  public:
    explicit GridTable(tuple_type&,tuple_type&,tuple_type*,int,const string,Domain *d=NULL);

    virtual void rewind();
    virtual bool pop();
    virtual bool done();
  
  private:
    tuple_type *mp_min,*mp_max,*mp_gridp;

    long dim,samp_per_dim,samp_total,index;
    tuple_type::value_type offset;
  };

template <class T>
GridTable<T>::GridTable
  (
  tuple_type *p_minimum,
  tuple_type *p_maximum,
  int_type *p_samples,
  tuple_type *p_gridpoint,
  int mode,
  const string& name,
  Domain *d=NULL
  )
  : T(mode,name,d)
  , mp_min(p_minimum)
  , mp_max(p_maximum)
  , mp_gridp(p_gridpoint)
  {
  INVARIANT((mp_min->val().rows()>0) && (p_samples->val()>0),"");
  /** Anzahl der Daten n pro Dimension (insgesamt also n^dim)
      festlegen, wobei n^dim < m_samples gelten soll */
  dim = mp_min->val().rows();
  int samp_per_dim = (int) (exp(log(p_samples->val())/dim)+0.5);
  int samp_total = (int) (pow(samp_per_dim,dim)+0.5); 
  if (samp_total > p_samples->dim()) 
    { --samp_per_dim; samp_total=static_cast<int>(pow(samp_per_dim,dim)); }

  /** Offset in den Achsen berechnen */
  offset.alloc(dim); 
  for (int i=0;i<dim;++i) 
    { offset[i]=(mp_max->val()[i]-mp_min->val()[i])/(samp_per_dim-1.0); }

  /** Wir beginnen mit dem ersten Gridpoint. */
  index = 0;
  }

template <class T>
void
GridTable<T>::rewind()
  { 
  index=0; 
  T::rewind(); 
  }

template <class T>
bool
GridTable<T>::done()
  {
  return (T::done()) && (index=samp_total-1);
  }

template <class T>
bool
GridTable<T>::pop()
  {
  mp_gridp->var().alloc(dim);
  matrix_set(mp_gridp->var(),mp_min->val());
  int i = index;
  int d = 0;
  do
    {
    mp_gridp->var()[d] += (i%dim)*offset[d];
    i = i/dim; ++d;
    }
  while (i>dim)

  return T::pop();
  }









