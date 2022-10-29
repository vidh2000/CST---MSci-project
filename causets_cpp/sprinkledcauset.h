/// \authors Vid Homsak, Stefano Veroni
/// \date 28/09/2022

#ifndef SPRINKLEDCAUSET_H
#define SPRINKLEDCAUSET_H

#include <set>
#include <vector>

#include "causetevent.h"
#include "causet.h"
#include "spacetimes.h"
#include "shapes.h"

/**
 * @brief An causet obtained from sprinkling in a spacetime subset 
 *        of a specified shape.
 */
class SprinkledCauset: public Causet
{
    public:

        // Public Attributes
        int Label;
        std::set<CausetEvent> _prec;
        std::set<CausetEvent> _succ;
        std::vector<double> _coordinates;
        std::vector<double> _position;

        // Constructor
        SprinkledCauset(int label = -1,
                    std::set<CausetEvent> past = {},
                    std::set<CausetEvent> future = {}, 
                    std::vector<double> position = {},
                    std::vector<double> coordinates = {});
        
        // "Getters"
        std::set<CausetEvent>& Past = _prec;
        std::set<CausetEvent>& Future = _succ;
        std::set<CausetEvent> Cone();
        std::set<CausetEvent> PresentOrPast();
        std::set<CausetEvent> PresentOrFuture();
        std::set<CausetEvent> Spacelike(std::set<CausetEvent> eventset);
        int PastCard();
        int FutureCard();
        int ConeCard();
        int SpacelikeCard(std::set<CausetEvent> eventset);
    
        //Overloading Operators
        bool operator ==(const CausetEvent &other) const;
        bool operator <(const CausetEvent &other) const;
        bool operator <=(const CausetEvent &other) const;
        bool operator >(const CausetEvent &other) const;
        bool operator >=(const CausetEvent &other) const;

        //Active Functionalities
        bool _addToPast(CausetEvent other);
        bool _addToFuture(CausetEvent other);
        bool _discard(CausetEvent other);
        void disjoin();

        //Relational Booleans
        bool isCausalTo(CausetEvent other);
        bool isSpacelikeTo(CausetEvent other);

        //Embedding Functionalities
        void embed(std::vector<double> coordinates, bool reembed = false);
        void disembed();
        bool isEmbedded();
        std::vector<double> Coordinates();
        int CoordinatesDim();

        // Hasse diagram functionalities
        std::vector<double> Position();
        void SetPosition(std::vector<double> value);





};

#endif /* SPRINKLEDCAUSET_H */