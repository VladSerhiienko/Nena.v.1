class Ft { 
  public ComplexNumber[] x; // inputs
  public ComplexNumber[] X; // outputs
  protected int N;
  
  public Ft() { 
  }
  
  public Ft(ComplexNumber[] x) { 
    this();
    this.x = x;
    Run();
  }
  
  public void Run() { 
    if (x == null) return;
    
    N = x.length;    
    X = new ComplexNumber[N];
    
    final float pi2 = 3.14159f * 2.0f;
    final float pi2N = pi2 / N;

    for (int k = 0; k < N; k++) { 
      ComplexNumber xn = null;      
      final float pi2Nk = pi2N * k;
      
      float XkRe = 0; 
      float XkIm = 0;
      
      for (int n = 0; n < N; n++) {        
        final float pi2Nkn = pi2Nk * n;
        xn = x[n];
        
        XkRe += xn.Re * cos(pi2Nkn);
        XkIm -= xn.Im * sin(pi2Nkn);        
      } // for loop n    
      
      X[k] = new ComplexNumber(XkRe, XkIm);      
    } // for loop k
  } // method Run()
  
} // class Ft
