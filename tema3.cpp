#include <iostream>
#include <memory>
#include <mutex>
using namespace std;

mutex mx;
class Uncopyable {
   protected:
     Uncopyable(){}
     ~Uncopyable(){}
   private:
     Uncopyable(const Uncopyable&);
     Uncopyable &operator =( const  Uncopyable&); 
};
class Doctor{

    protected:
        std::string type ;
        std::string name;
        bool value;
    public:
       
        Doctor( std::string  name1, std::string  typ, bool value1) :
            type(typ),
            name(name1),
            value(value1)
         {
           // std::cout <<"constructor 2:"<<type<<" "<<name<<std::endl; 
         }
        
        Doctor(const Doctor &copy):name(copy.name) , type(copy.type),value(copy.value)
        {
            // std::cout <<"copy constructor <<std::endl;
        }

        ~Doctor()
        {
            std::cout << "delete" << std::endl;
        }
     
        Doctor& operator =(const Doctor& doctor1)
        {
             if(this == &doctor1)
             {
                std::cout<<"identical"<<std::endl;
                return *this;
             }

             this->name=doctor1.name;
             this->type = doctor1.type;
             this->value= doctor1.value;
            return *this;
        }
        
        void print()
        {
            std::cout<<"-name: "<<name<< "-> tipul: "<< type << "-> working day: "<< value<<std::endl;
        }

        bool AfisareDoctorLocked()   //functia afiseaza daca doctorul lucreaza sau nu in ziua respectiva prin parametrul "value"
        {
            return this->value;
        }

        void SetareDoctorLocked()    //functia seteaza daca doctorul va lucra sau nu intr-o anumita zi prin setarea parametrului "value " pe TRUE/FALSE;
        {
             if(value ==true)             //daca doctorul la inceput nu lucreaza in ziua data adica value = false putem sa ii setam ziua repectiva ca lucratoare punand valoarea lui "value " pe TRUE si invers;
             {
               this->value = false;
             }
             else
             {
                this->value = true;
             }
        }

        std::string getName(){
            return this->name;
        }
};


Doctor*  newDoctor(const std::string &name ,const std::string &type, const bool &value) 
{
    return new Doctor(name,type,value);
}

void AfisareNewDoctor(auto_ptr<Doctor> & doctor)
{
    doctor->print();
}

void AfisareNewDoctor(shared_ptr<Doctor> & doctor)
{
    std::cout<< doctor.use_count() << " number of instances for this resource" << std::endl;   
    doctor->print();
}


void lock(Doctor &doctor) //function to block a doctor
{
  doctor.SetareDoctorLocked();
  std::cout<<"Block ->doctorul va lucra in ziua data :" <<doctor.AfisareDoctorLocked()<<std::endl;
}
void unlock(Doctor &doctor)   //function to unblock a doctor
{
    doctor.SetareDoctorLocked();
  std::cout<<"Unblock "<<std::endl;
}

class LockedDoctor :private  Uncopyable
{
    private:
        mutex *mutexPtr;
        Doctor doctor;
    public:
        explicit LockedDoctor(mutex *pm, Doctor& doctor)
        : mutexPtr(pm), 
          doctor(doctor)
        { 
            mutexPtr->lock();
            lock(doctor);
        }
        ~LockedDoctor() 
        { 
            mutexPtr->unlock(); 
            unlock(doctor);
        } 
};
int main()
{
   
    Doctor doctor1("Floroiu","veterinar",true);                                          //false(0) ->zi libera
    doctor1.print();                                                                  //true(1)  ->zi lucratoare
    Doctor doctor2("Iulia","cardiolog",false);


    //item 13. Use objects to manage resources
    //auto_ptr smart pointer 

    std::auto_ptr<Doctor> doc1(newDoctor("Denisa","med veterinar",false));   //doc1 points to the object returned from Doctor
    doc1->print(); 
    std::auto_ptr<Doctor> doc2(newDoctor("Maria","med stomatolog",false));   //doc2 points now to the object, doc1 is null
    doc2->print(); 
    doc1=doc2; // doc1 points to the object,doc2 is null
    doc1->print();
    //doc2 ->print() compilation error


    //share_ptr
   {
    std::shared_ptr<Doctor> dockie1(newDoctor("Bia","med  cardiolog",false));
      AfisareNewDoctor(dockie1);
     {
            std::shared_ptr<Doctor> dockie2 = dockie1;
                  AfisareNewDoctor(dockie2);
     }
           AfisareNewDoctor(dockie1);
    }  
         //  AfisareNewDoctor(dockie1);


    // AfisareNewDoctor(doc1);

    //item 14.  Think carefully about copying behavior in resource-managing classes.
    std::cout<<"Afisare daca doctorul lucreaza in ziua data :"<< doctor1.AfisareDoctorLocked()<<std::endl;
    LockedDoctor doc6(&mx, doctor1);
    doctor1.print();


return 0;
}