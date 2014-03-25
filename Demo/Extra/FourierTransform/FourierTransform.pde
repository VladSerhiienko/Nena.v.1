
Ft fourierTransform = null;

void setup() { 
  
  print("Inputs:\n");
  
  int numInputs = 20;
  ComplexNumber[] inputs = new ComplexNumber[numInputs];  
  for (int i = 0; i < inputs.length; i++) { 
    inputs[i] = new ComplexNumber(
      random(-10, 10), 
      random(-10, 10)
      );
      
      print(inputs[i] + "\n");
  }
  
  print("Outputs:\n");
  
  fourierTransform = new Ft(inputs);  
  for (int i = 0; i < fourierTransform.X.length; i++) { 
      print(fourierTransform.X[i].toString() + "\n");
  }
  
}
  
int ilen = fourierTransform.x.length;
int olen = fourierTransform.X.length;

void RedrawFt() { 
  
  for (int i = 0; i < olen; i++) { 
    
    
  }
  
  
  
}

void draw() { 
  // background(0);
}
