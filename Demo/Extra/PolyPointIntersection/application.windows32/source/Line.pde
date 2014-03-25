class Line { 
  public float k, b; // kx + b = y  
  public String name;
  
  public void FromPoints( Point a, Point b ) { 
    float dx = (b.x - a.x);
    if (dx != 0) this.k = (b.y - a.y) / dx;
    else this.k = 0.0f;
    this.b = a.y - a.x * this.k;
  }   
  public void CopyFrom( Line other ) { 
    this.name = other.name;
    this.k = other.k;
    this.b = other.b;
  }
  public boolean IsSameAs( Line other ) { 
    return other.k == this.k && other.b == this.b;
  }
  public boolean IsParallelTo( Line other ) { 
    return other.k == this.k;
  }
  public boolean IsPerpendicularTo( Line other ) { 
    return -1 == other.k * this.k;    
  }
  public boolean Crosses( Line other ) { 
    return other.k != this.k;
  } 
  public float Substitute( Point p ) { 
    return this.k * p.x + this.b;
  }
  public Point CrossPointOf( Line line ) { 
    if (this.IsParallelTo(line)) return null; else { 
      Point crossPoint = new Point();
      crossPoint.x = (line.b - this.b) / (this.k - line.k);
      crossPoint.y = this.Substitute(crossPoint);
      return crossPoint;
    }
  } 
  public String toString() { 
    return "Line: " + name + "(" + this.k + ", " + this.b + ")";
  }
}
