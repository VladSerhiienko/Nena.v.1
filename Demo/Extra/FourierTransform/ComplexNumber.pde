class ComplexNumber { 
  public float Re; // real term
  public float Im; // imaginary term
  
  public ComplexNumber() { 
    this.Re = 0;
    this.Im = 1;
    // = 0 + 1i
  }
  
  public ComplexNumber(final float Re, final float Im) { 
    this.Re = Re;
    this.Im = Im;
  }
  
  public void Add(final ComplexNumber other) { 
    this.Re += other.Re; 
    this.Im += other.Im;
  }
  
  public void Sub(final ComplexNumber other) { 
    this.Re -= other.Re; 
    this.Im -= other.Im;
  }
  
  public void Mul(final ComplexNumber other) { 
    this.Re *= other.Re; 
    this.Im *= other.Im;
  }
  
  public void Div(final ComplexNumber other) { 
    this.Re /= other.Re; 
    this.Im /= other.Im;
  }
  
  public String toString() { 
    return "(" + this.Re + ")+(" + this.Im + ")i";
  }
  
}
