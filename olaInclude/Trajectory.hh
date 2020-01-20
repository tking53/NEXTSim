//
// $Id: Trajectory.hh 12.12.2016 A. Fijalkowska $
//
/// \file Trajectory.hh
/// \brief Definition of the Trajectory class based on example optical/LXe

#ifndef Trajectory_h
#define Trajectory_h 1

#include "G4Trajectory.hh"
#include "G4Allocator.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4TrajectoryPoint.hh"
#include "G4Track.hh"
#include "G4Step.hh"

class G4Polyline;  

class Trajectory : public G4Trajectory
{
  public:

    Trajectory();
    Trajectory(const G4Track* aTrack);
    Trajectory(Trajectory &);
    virtual ~Trajectory();
 
    virtual void DrawTrajectory() const;
 
    inline void* operator new(size_t);
    inline void  operator delete(void*);

    void SetDrawTrajectory(G4bool b){drawTraj = b;}
    void WLS(){ifWls=true;}
    void SetForceDrawTrajectory(G4bool b){forceDrawTraj = b;}
    void SetForceNoDrawTrajectory(G4bool b){forceNoDrawTraj = b;}

  private:

    G4bool ifWls;
    G4bool drawTraj;
    G4bool forceNoDrawTraj;
    G4bool forceDrawTraj;
    G4ParticleDefinition* particleDefinition;
};

extern G4ThreadLocal G4Allocator<Trajectory>* TrajectoryAllocator;

inline void* Trajectory::operator new(size_t)
{
  if(!TrajectoryAllocator)
      TrajectoryAllocator = new G4Allocator<Trajectory>;
  return (void*)TrajectoryAllocator->MallocSingle();
}

inline void Trajectory::operator delete(void* aTrajectory)
{
  TrajectoryAllocator->FreeSingle((Trajectory*)aTrajectory);
}

#endif
