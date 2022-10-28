#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <stack>
#include <string>
#include <stdio.h>
#include <vector>
#include <set>
#include <stdexcept>

#include <D:\Documents\Sola\Imperial College London\Year 4\MSci project\Project\causets_code\causets_cpp\functions.h>


///////////////////////////////////////////////////////////////////////////////
// Useful functions for set manipulations
//
//template <typename obj>  
//void print_set(std::set<obj> set){
//    /*
//    Prints elements of a set one by one.
//    */
//    std::cout << "{ ";
//    for (obj e : set)
//    {
//        std::cout << e << ' , ';
//    }
//    std::cout << " }";
//    std::cout << std::endl;}
//
//bool set_contains(CausetEvent element, std::set<CausetEvent> s)
//    {
//    bool is_in = s.find(element) != s.end();
//    return is_in;}
//
//std::set<CausetEvent> set_diff(std::set<CausetEvent> s1,
//                               std::set<CausetEvent> s2)
//    /*
//    Return set difference i.e events which are only in s1;
//    RETURN = s1-s2 (where s2 can have other elements as well)
//    */
//    {
//    std::set<CausetEvent> result;
//    std::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(),
//        std::inserter(result, result.end()));
//    return result;}
//
//std::set<CausetEvent> set_union(std::set<CausetEvent> s1,
//                                std::set<CausetEvent> s2)
//    /**
//    * @brief Returns union of two sets s1 and s2.
//    */
//    {
//    s1.insert(s2.begin(), s2.end());
//    return s1;}
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// CausetEvent class for used for all the causal set elements
//////////////////////////////////////////////////////////////////////////////
class CausetEvent
{
    /*
    Handles a single event (point) and its causal relations in a causal set.
    The attribute 'Label' can be used to assign a label, but does not need to 
    be unique (default: None).

    Instances of CausetEvent are comparable:
    a == b             is True if a is the same instance as b
    a < b              is True if a precedes b
    a <= b             is True if a precedes b or a is the same as b
    a > b              is True if a succeeds b
    a >= b             is True if a succeeds b or a is the same as b
    a.isSpacelikeTo(b) is True if a is spacelike separated to b
    
    */

    // Make everything public (for now)
    public:
    // Public Attributes
    int Label;
    std::set<CausetEvent> _prec;
    std::set<CausetEvent> _succ;
    std::vector<double> _coordinates;
    std::vector<double> _position;

    /** 
    * @Constructor:
    * Initialise a CausetEvent.
    * 
    *    Keyword parameters:
    * label: str
    *    Label for the event (does not need to be unique in a causet)
    * past: Iterable[CausetEvent]
    *    Set of past events (that may or may not be linked).
    *    This instance will automatically be added to their future.
    * future: Iterable[CausetEvent]
    *    Set of future events (that may or may not be linked).
    *    This instance will automatically be added to their past.
    * coordinates: Iterable[float]
    *    Coordinates if the event shall be considered as embedded in a 
    *    spacetime region.
    * position: Iterable[float]
    *    Coordinate pair of the event in a Hasse diagram if the Hasse 
    *    diagram is manually defined.
    **/
    CausetEvent(int label = -1,
            std::set<CausetEvent> past = {},
            std::set<CausetEvent> future = {}, 
            std::vector<double> position = {},
            std::vector<double> coordinates = {}
            )
    {
        this -> Label = label;
        _prec = {};
        for (CausetEvent e : past){
            std::set<CausetEvent> presOrPast = e.PresentOrPast();
            _prec = set_union(_prec, presOrPast);} 
        _succ = {};
        for (CausetEvent e : future){
            std::set<CausetEvent> presOrFut = e.PresentOrFuture();
            _succ = set_union(_succ, presOrFut);}
        this -> _coordinates = coordinates;
        this -> _position = position;
        // Add this instance to its causal relatives:
        for (CausetEvent e : _prec)
        {   e._addToFuture(*this);}
        for (CausetEvent e : _succ)
        {   e._addToPast(*this);}
    }

    // Overloading ==, <, <=, >, >= to relate CausetEvent instances
    
    bool operator == (CausetEvent other){
        return other.Label == Label;}
        
    bool operator < (CausetEvent other){
        return set_contains(other, _succ);}

    bool operator <= (CausetEvent other){
        return (*this==other or set_contains(other, _succ));} //"required from here" error?
    
    bool operator > (CausetEvent other){
        return set_contains(other, _prec);}

    bool operator >= (CausetEvent other){
        return (*this==other or set_contains(other, _prec));}
        
    
    ////////////////////////////////////////////////////////////
    //ACTIVE FUNCTIONALITIES
    ////////////////////////////////////////////////////////////
    bool _addToPast(CausetEvent other)
    {
        /*
        Adds an event to the past of this event.
        It returns False if the event is already in the past,
        otherwise it adds the event and return True.
        (implemented without link referenceses)
        */

        if (set_contains(other, _prec)) //? gives warning/error: "required from here" ?
        {
            return false;
        }
        else
        {
            //if (hasBeenLinked() && isLink(other, *this))
            //{
            //    _lprec = set_diff(_lprec, other._prec);
            //    _lprec.insert(other);
            //}
            _prec.insert(other);
            return true;
        }
    }

    bool _addToFuture(CausetEvent other){
        /*
        Adds an event to the future of this event.
        It returns False if the event is already in the future,
        otherwise it adds the event and returns True.
        (implemented without link referenceses)
        */
        if (set_contains(other,_succ))
        {
            return false;
        }
        else
        {
            //if (hasBeenLinked() && isLink(other, *this))
            //{
            //    _lsucc = set_diff(_lsucc, other._succ);
            //    _lsucc.insert(other);
            //}
            _succ.insert(other);
            return true;
        }}

    bool _discard(CausetEvent other){
        /*
        Removes an event from the past and future of this event.
        It returns True if the event was in the past or future,
        otherwise False.
        (implemented without link referenceses)
        */
        if (set_contains(other, _prec))
        {   
            _prec.erase(other);
            return true;
        }
        else if (set_contains(other, _succ))
        {
            _succ.erase(other);
        }
        else
        {
            return false;
        }}


    void disjoin(){
        /*
        Disjoins this event from the causal set (the set of events to the past 
        and future).
        (implemented without link referenceses)
        */
        //unlink()
        for(CausetEvent e : Cone())
        {
            e._discard(*this);
        }
        _prec = {};
        for(CausetEvent e : Cone())
        {
            e._discard(*this);
        }
        _succ = {};
        }

    /*void link(){
        
        Computes the causal links between this event and all related events.

        Only call this method if it is necessary to increase performance of 
        this instance, when link requests with isPastLink(...) and 
        isFutureLink(...) are necessary. The first call of any of these two 
        methods will call link() if it was not called before.
        
        std::set<CausetEvent> _lprec;
        for (CausetEvent e = _prec.begin(); _prec.end(); e++)
        {
            if (isLink(e, *this))
            {
                _lprec.insert(e)
            }
        }
        std::set<CausetEvent> _lsucc;
        for (CausetEvent e = _succ.begin(); _succ.end(); e++)
        {
            if (isLink(e,*this))
            {
                _lsucc.insert(e)
            }
        }}*/
    /*void unlink(){
        
        Force the CausetEvent instance to reset its link memory.
        
    }*/
    //CausetEvent copy(){}
    //double Rank(){}

    ////////////////////////////////////////////////////////////
    // RELATIONAL BOOLEANS 
    ////////////////////////////////////////////////////////////

    //bool hasBeenLinked(){}
    //static bool isLink(){}
    //bool isPastLink(){}
    //bool isFutureLink(){}

    bool isCausalTo(CausetEvent other){
        /*
        Tests if another event is causally related to this event.
        */
        return (*this<=other)|| (*this>other);}

    //bool isLinkedTo(){}

    bool isSpacelikeTo(CausetEvent other){
        /*
        Tests if another event is spacelike separated to this event.
        */
       return ((other.Label != Label) && set_contains(other,_prec))
                    && set_contains(other,_succ);
    }

    ////////////////////////////////////////////////////////////
    // PAST, FUTURE and SPACELIKE
    ////////////////////////////////////////////////////////////

    //static int LinkCountOf(){}

    /* Returns a set of events (instances of CausetEvent)
    that are in the past of this event.*/
    std::set<CausetEvent>& Past = _prec;
    

    /*Returns a set of events (instances of CausetEvent)
    that are in the future of this event.*/
    std::set<CausetEvent>& Future = _succ;


    std::set<CausetEvent> Cone(){
        /*
        Returns a set of events (instances of CausetEvent)
        that are in the past, present or future of this event.
        */
        std::set<CausetEvent> Cone = set_union(_prec,_succ);
        Cone.insert(*this);
        return Cone;}
    

    std::set<CausetEvent> PresentOrPast(){
        /*
        Returns a set of events (instances of CausetEvent) 
        that are in the past of this event, including this event.
        */
        std::set<CausetEvent> presOrPast = _prec;
        presOrPast.insert(*this);
        return presOrPast;}


    std::set<CausetEvent> PresentOrFuture(){
        /*
        Returns a set of events (instances of CausetEvent) 
        that are in the future of this event, including this event.
        */
        std::set<CausetEvent> presOrFut = _succ;
        presOrFut.insert(*this); //?
        return presOrFut;}


    std::set<CausetEvent> Spacelike(std::set<CausetEvent> eventset){
        /*
        Returns the subset of events (instances of CausetEvent) of
        `eventSet` that are spacelike separated to this event.
        */
        return set_diff(eventset, Cone());} // "required from here" error?


    int PastCard(){
        /*
        Returns the number of past events (set cardinality).
        */
        return _prec.size();}


    int FutureCard(){
        /*
        Returns the number of future events (set cardinality).
        */
        return _succ.size();}


    int ConeCard(){
        /*
        Returns the number of past and future events (set cardinality), 
        including this event (present).
        */
        return _prec.size()+_succ.size()+1;}


    int SpacelikeCard(std::set<CausetEvent> eventset){
        /*
        Returns the number of events (instances of CausetEvent) of
        `eventSet` that are spacelike separated to this event.
        */
        if (set_contains(*this, eventset))
        {
            std::set<CausetEvent> s;
            s = set_diff(set_diff(eventset, _prec),_succ);
            return s.size()-1;
        }
        else
        {   
            std::set<CausetEvent> s;
            s = set_diff(set_diff(eventset, _prec),_succ);
            return s.size();
        }}


    //std::set<CausetEvent> LinkPast(){}
    //std::set<CausetEvent> LinkFuture(){}
    //std::set<CausetEvent> LinkCone(){}
    //int LinkPastCard()
    //int LinkFutureCard()
    //int LinkConeCard()

    ////////////////////////////////////////////////////////////
    // EMBEDDING FUNCTIONALITIES
    ////////////////////////////////////////////////////////////
    void embed(std::vector<double> coordinates, bool reembed = false){
        /*
        Assigns coordinates to the event. If the event already has coordinates, 
        an `AttributeError` is raised. To avoid this error and overwrite the 
        value, use reembed.
        */
        if (not reembed and this->isEmbedded()) //?
        {   
            throw std::invalid_argument(
                "The event is already embedded. Use the disembed method first or use the reembed flag.");
        }
        _coordinates = coordinates;}
    void disembed(){
        /*
        Removes the embedding of the event,
        e.g. sets _coordinates = {}
        */
        _coordinates = {};}

    std::vector<double> Position(){
        /*
        Returns the coordinates for the position in a Hasse diagram.
        */
        try
        {
            return _position;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return {0.0,0.0};
        }}


    void SetPosition(std::vector<double> value){
        /*
        Sets the coordinates for the position in a Hasse diagram.
        */
        if (value.size() !=2)
        {
            throw std::invalid_argument(
                "The position has to be an iterable of exactly two values.");
        }
        _position = value;
    }
    bool isEmbedded(){
        /*
        Returns True if the event has a coordinate tuple assigned to it,
        e.g. if the coordinates tuple has non-default value.
        */
        if (_coordinates.size() != 0)
        {
            return true;
        }
        else
        {
            return false;
        }}

    std::vector<double> Coordinates(){
        /*
        Returns the embedding coordinates.

        Raises an ?AttributeError? if the event is not embedded.
        ^ to be determined
        */
        try
        {
            return _coordinates;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            std::vector<double> default_vec = {0};
            return default_vec; //the python here returns int 0...
        }}
        

    int CoordinatesDim(){
        /*
        Returns the dimension of its coordinates if any, otherwise 0.
        */
        try
        {
            return _coordinates.size();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return 0;
        }}

    // Destructor
	~CausetEvent();

};

int main(){

int a = 3;
std::cout << a;

};







