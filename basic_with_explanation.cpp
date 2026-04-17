#include <iostream>

//Create example functions to print different things.
namespace ExampleFunctions {
    void example_ba() {
        std::cout << "Base_A\n";
    }
    void example_bb() {
        std::cout << "Base_B\n";
    }

    void example_na() {
        std::cout << "New_A\n";
    }
    void example_nb() {
        std::cout << "New_B\n";
    }
}

/* The virtual table is the way that real programs lookup virtual function pointers.

When a base class is defined, it has a vtable filled with the basic functions.
When a derived class is created, it has a vtable of the same size (unless it defines new virtual functions). 
Assuming it does not, the derived class' virtual table will be different based on which functions it chooses to overwrite.
As you will see, our example inherited class only chooses to overwrite function A, not function B.

*/
typedef struct vtable {
    void(*function_A)();
    void(*function_B)();
};

/* Vtables are defined at compile time and are kept constant for every class. That means that two instances of "ClassA" have the EXACT same vTable pointer */
static const vtable base_vtable = { &ExampleFunctions::example_ba, &ExampleFunctions::example_bb };
static const vtable inheritor_vtable = { &ExampleFunctions::example_na, &ExampleFunctions::example_bb };


//Define our "classes"

/* This is the base class. It has a full vtable and a function to call either one. */
typedef struct Base {
    void call_function(bool ab) {
        if (ab) {
            v->function_A();
        }
        else {
            v->function_B();
        }
    }

    const vtable* v; //This pointer truly cannot change.
};

/* Replicate everything in MEMORY. The constructor function is where things change. */
typedef struct Inheritor : Base {
};

//Construct a base. Here, we set A to example_a and B to example_b. 

/* heres how this would look as an actual class

class Base {
public:
    virtual void function_A(){
    ...
    }

    virtual void function_B(){
    ...
    }
};
*/

Base create_base() {
    Base b;
    b.v = &base_vtable;
    
    return b;
}



/* heres how this would look as an actual class

class Inheritor : public Base {
    void function_A() override {
        //Only choose to overwrite the first function A
    }
};
*/
Inheritor create_inherited_class() {
    Inheritor i; //Create our inheritor class.

    //Set the vtable as the *inheritor* vtable.
    i.v = &inheritor_vtable;
   
    
    return i;
}

#define FUNCTION_A 1
#define FUNCTION_B 0

int main()
{
    //Simply construct our two structs.
    Base b = create_base();
    Inheritor i = create_inherited_class();

    //This is where we begin to understand the functionality of the vtable.

    /* 
    It's important to recall the following facts
        - The Inheritor struct inherits everything from the Base struct. Its memory layout is identical ONLY BECAUSE we chose not to add anything additional.
        - The Base struct's vtable points function_A at "Base_A" and function_B at "Base_B"
        - The Inheritor's struct vtable points function_A at "New_A" but does NOT overwrite function_B, so that points at "Base_B" still.
        - NO VTABLES were created after compilation. These two structs are dealing with predefined vtables.

    Here, we create a Base* pointer; HOWEVER, we are casting an Inheritor* pointer to create it. 
    In reality, you may have many usecases for this. The important thing to remember is that we are calling Base functions
    but using the Inheritor structs vtable. Watch what happens.
    */
    Base* bptr = (Base*)(&i); 

    bptr->call_function(FUNCTION_A); /*Call function_A --> Outputs "New_A". When constructing Inheritor i, we used the inheritor vtable which has a different
                                     value for function_A.
                            */

    bptr->call_function(FUNCTION_B); /*Call function_B --> Outputs "Base_B". function_B is stored as the *original* because we chose not to overwrite it when 
                                     defining the Inheritor, instead using the Base class' function_B. */
}

