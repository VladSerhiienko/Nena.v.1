class Point { 
  public float r; // circle radius (rendering)
  public float x, y; // point coordinates 
  public String name; // point name
  
  public Point() { 
    this.r = 5;
    this.x = 0;
    this.y = 0;
  }
  
  public boolean BelongsTo(final Line line) { 
    return SubstituteIn(line) == this.y;
  }  
  public float SubstituteIn(final Line line) { 
    return this.x * line.k + line.b;
  }  
  public void CopyFrom(final Point other) { 
    this.name = other.name;
    this.r = other.r;
    this.x = other.x;
    this.y = other.y;
  }
  public float DistanceTo(final Point other) { 
    Vector direction = new Vector(this, other);
    return direction.Magnitude();
  }
  public float DistanceTo(final float x, final float y) { 
    Vector direction = new Vector(x - this.x, y - this.y);
    return direction.Magnitude();
  }
  public boolean IsMouseOver(final float mx, final float my) { 
    Point mouse = new Point();
    mouse.x = mx; mouse.x = my;
    return this.DistanceTo(mx, my) <= r;    
  }
  public boolean IsMouseOver(final Point mouse) { 
    return this.DistanceTo(mouse) <= r;
  }
  public String toString() { 
    return "Point: " + name + "(" + this.x + ", " + this.y + ")";
  }
  public void Draw() { 
    ellipse(this.x, this.y, this.r, this.r);    
  }
}
