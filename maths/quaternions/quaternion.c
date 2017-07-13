#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct quaternion {

  double real;
  double i;
  double j;
  double k;
} quaternion;


quaternion create_quaternion(double real, double i, double j, double k){

  quaternion result;

  result.real = real;
  result.i = i;
  result.j = j;
  result.k = k;

  return result;
}


quaternion add_quaternions(quaternion a, quaternion b){

  quaternion result;

  result.real = a.real + b.real;
  result.i = a.i + b.i;
  result.j = a.j + b.j;
  result.k = a.k + b.k;

  return result;

}

quaternion conjugate_quaternion(quaternion a){

  quaternion result;
  result.real = 0.0;
  result.i = -a.i;
  result.j = -a.j;
  result.k = -a.k;

  return result;
}

quaternion inverse_quaternion(quaternion a){
  //Implement
  return a;
}
  
quaternion multiply_quaternion_s(quaternion a,double s){

  quaternion result;

  result.real = a.real * s;
  result.i = a.i * s;
  result.j = a.j * s;
  result.k = a.k * s;

  return result;
}


quaternion multiply_quaternions(quaternion a, quaternion b){

  // i^2 = j^2 = k^2 = -1
  // ij = -ji = k
  // jk = -kj = i
  // ki = -ik = j

  quaternion result;

  result.real = (a.real * b.real) - (a.i * b.i) - (a.j * b.j) - (a.k * b.k);
  result.i = (a.i * b.real) + (a.real * b.i) + (a.j * b.k) - (a.k * b.j);
  result.j = (a.j * b.real) + (a.real * b.j) + (a.k * b.i) - (a.i * b.k);
  result.k = (a.k * b.real) + (a.real * b.k) + (a.i * b.j) - (a.j * b.i);

  return result;

  
}

quaternion rotate_quaternions(quaternion point, quaternion axis, double angle){

  //Rotation rule of quaternions:
  // p' = qp(q^-1)
  double half_ang = angle/2;
  quaternion p,q,q_conj,result;

  double vector_mag = 1/sqrt((axis.i*axis.i)+(axis.j*axis.j)+(axis.k*axis.k));
  
  p = point;
  q = create_quaternion(cos(half_ang),axis.i*sin(half_ang)*vector_mag,
			axis.j*sin(half_ang)*vector_mag, axis.k*sin(half_ang)*vector_mag);
  q_conj = conjugate_quaternion(q);

  
  
  result = multiply_quaternions(p,q);
  result = multiply_quaternions(result,q_conj);
  


  return result;

  
}


int main() {

  quaternion a,b,c;

  //a = 3 -4i + 2j + 5k
  a = create_quaternion(3,-4,2,5);
  //b = 2 - 3i - 7j + 3k
  b = create_quaternion(2,-3,-7,3);
  // c = a * b 
  c = multiply_quaternions(a,b);

  printf("Multiply Quaternions: c = a*b \n\n");
  printf("a = %f + %fi + %fj + %fk \n",a.real,a.i,a.j,a.k);
  printf("b = %f + %fi + %fj + %fk \n",b.real,b.i,b.j,b.k);
  printf("c = %f + %fi + %fj + %fk \n",c.real,c.i,c.j,c.k);
  printf("\n");

  //Lets try rotating a point in 3d space about an axis, all in quaternions!!

  quaternion axis = create_quaternion(0,1,0,0);
  quaternion point = create_quaternion(0,0,2,0);

  quaternion rotated = rotate_quaternions(point,axis,1.508);
  printf("Rotate Point about Axis using Quaternions: p' = qpq^-1 \n\n");
  printf("axis = %f + %fi + %fj + %fk \n",axis.real,axis.i,axis.j,axis.k);
  printf("point = %f + %fi + %fj + %fk \n",point.real,point.i,point.j,point.k);
  printf("rotated = %f + %fi + %fj + %fk \n",rotated.real,rotated.i,rotated.j,rotated.k);

  
  return 0;

}
