#include <Kokkos_Core.hpp>
#include <iostream>
class Foo {
 protected:
  int val;

 public:
  KOKKOS_FUNCTION
  Foo() { val = 0; };

  KOKKOS_FUNCTION
  virtual int value() { return 0; };

  KOKKOS_FUNCTION
  void set_value(double v) { val = v; }

  KOKKOS_FUNCTION
  virtual ~Foo() {}
};

class Foo_1 : public Foo {
 public:
  KOKKOS_FUNCTION
  Foo_1() { val = 1; };

  KOKKOS_FUNCTION
  int value() { return val; };
};

class Foo_2 : public Foo {
 public:
  KOKKOS_FUNCTION
  Foo_2() { val = 2; };
  KOKKOS_FUNCTION
  int value() { return val; };
};

int main(int argc, char* argv[]) {
  Kokkos::initialize(argc, argv);

  {
    
    Foo* f_1 = (Foo*)Kokkos::kokkos_malloc(sizeof(Foo_1));
    Foo* f_2 = (Foo*)Kokkos::kokkos_malloc(sizeof(Foo_2));

    Kokkos::parallel_for(
        "CreateObjects", 1, KOKKOS_LAMBDA(const int&) {
          new ((Foo_1*)f_1) Foo_1();
          new ((Foo_2*)f_2) Foo_2();
        });

    int value_1, value_2;
    Kokkos::parallel_reduce(
        "CheckValues", 1,
        KOKKOS_LAMBDA(const int&, int& lsum) { lsum = f_1->value(); }, value_1);

    Kokkos::parallel_reduce(
        "CheckValues", 1,
        KOKKOS_LAMBDA(const int&, int& lsum) { lsum = f_2->value(); }, value_2);

    printf("Values: %i %i\n", value_1, value_2);

    Kokkos::parallel_for(
        "DestroyObjects", 1, KOKKOS_LAMBDA(const int&) {
          f_1->~Foo();
          f_2->~Foo();
        });

    Kokkos::kokkos_free(f_1);
    Kokkos::kokkos_free(f_2);
  }

  Kokkos::finalize();
}
