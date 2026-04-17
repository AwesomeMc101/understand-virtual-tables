#include <iostream>
#include <cmath>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926
#endif

//Create example functions to print different things.
namespace ExampleFunctions {
    int add(int a, int b) {
        return (a + b);
    }

    float sin_rad(float a) {
        return sinf(a);
    }
    float sin_deg(float a) {
      
        a *= M_PI;
        a /= 180.0f;
        return sinf(a);
    }

}

/* The virtual table is the way that real programs lookup virtual function pointers.

When the radian (base) class is defined, it has a vtable filled with the add and sin_rad functions.

When the degree (inherited) class is created, it has a vtable of the same size (unless it defines new virtual functions). 
Assuming it does not define a new vfunction, the derived class' virtual table will be different based on which functions it chooses to overwrite.
As you will see, our example inherited class only chooses to overwrite the sin function pointer, not the add function. 

*/
typedef struct vtable {
    int(*add_function)(int a, int b);
    float(*sin_function)(float v);
};

/* Vtables are defined at compile time and are kept constant for every class. That means that two instances of "ClassA" have the EXACT same vTable pointer */
static const vtable rad_vtable = { &ExampleFunctions::add, &ExampleFunctions::sin_rad };
static const vtable deg_vtable = { &ExampleFunctions::add, &ExampleFunctions::sin_deg };


//Define our "classes"

/* This is the base reference class. It has a full vtable and both functions. */
typedef struct Radian_Calculator {
    int add(int a, int b) {
        return v->add_function(a, b);
    }
    float sin(float a) {
        return v->sin_function(a);
    }

    const vtable* v; 
};

/* Replicate everything in MEMORY. The constructor function is where things change. */
typedef struct Degree_Calculator : Radian_Calculator {
};



/* heres how this would look as an actual class

class Radian_Calculator {
public:
    virtual int add(int a, int b){
        return (a+b);
    }

    virtual float sin(float a){
        return sinf(a); 
    }
};
*/
Radian_Calculator create_rad_calc () {
    Radian_Calculator b;
    b.v = &rad_vtable;
    
    return b;
}



/* heres how this would look as an actual class

class Degree_Calculator : public Radian_Calculator {
    float sin(float a) override {
        a *= M_PI;
        a /= 180.0f;
        return sinf(a);
    }
};

BUT WAIT?! Where is the add() function?? Well, we inherit that virtual function from Radian_Calculator. Adding is the same... we don't need
to overwrite that function!! That's why the vtables BOTH point to ExampleFunctions::add().

*/
Degree_Calculator create_deg_calc() {
    Degree_Calculator i; //Create our degree class.

    //Set the vtable as the *degree* vtable.
    i.v = &deg_vtable;

    //In our vtable, the ADD function pointer does not change! There is no need to! So they can point to the exact same virtual function in memory.
   
    
    return i;
}

int main()
{
    //Simply construct our two structs.
    Radian_Calculator rad_calc = create_rad_calc();
    Degree_Calculator deg_calc = create_deg_calc();

    /* Now for this example, we will see practically the need for some functions to be virtualized. */
    // We have a Degree calculator but view it through the "lense" of a Radian calculator
    Radian_Calculator* calc = (Radian_Calculator*)(&deg_calc);

    //Now, this prints exactly 1. Even though 90 radians SHOULD print 0.893 ( sin(90 rad) )... this is because we are STILL using the degree calc's vtable.
    std::cout << calc->sin(90);

    //Now,
    std::cout << "\n" << calc->add(1, 5);
    //This prints 6. Regardless of what calculator we use. We didn't need to overwrite this virtual function because adding is the same
    //regardless of degrees or radians.
}

