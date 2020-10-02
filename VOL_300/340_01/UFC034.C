#include "_UFC03.H"
#include "testfmt.H"
main()
{ FKZ=2;
  SM=0;
  memcpy(FMT,"testfmt ",8);
  #include "testfmt.i"
  _UNIF(&FKZ,FMT,testfmt.fld0,&RET,&SM,Daten);
}
