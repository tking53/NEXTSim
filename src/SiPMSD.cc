//
// Created by David Pérez Loureiro on 1/10/17.
//

#include "SiPMSD.hh"
#include "G4ParticleTypes.hh"


SiPMSD::SiPMSD(G4String name) : G4VSensitiveDetector(name) {

    G4String HCname;
    collectionName.insert(HCname="SiPMCollection");
}

SiPMSD::~SiPMSD() {
;
}

void SiPMSD::Initialize(G4HCofThisEvent *HCE) {

    static int HCID = -1;
    hitsCollection = new SiPMHitsCollection
            (SensitiveDetectorName, collectionName[0]);
    if(HCID<0){
        HCID=GetCollectionID(0);
        HCE->AddHitsCollection(HCID,hitsCollection);
    }
}

G4bool SiPMSD::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist) {
    return false;
}

G4bool SiPMSD::ProcessHits_constStep(const G4Step *aStep, G4TouchableHistory *ROhist){

    //need to know if this is an optical photon
    if(aStep->GetTrack()->GetDefinition()
       != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

    G4int SipmNumber=
            aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber();
    //G4cout<<"SiPM number "<<SipmNumber<<G4endl;

    G4VPhysicalVolume* physVol=
            aStep->GetPostStepPoint()->GetTouchable()->GetVolume();

    //G4cout<<"SiPMSD::ProcessHits_constStep()-->"<<physVol->GetName()<<" "<<physVol->GetMultiplicity()<<G4endl;
    //G4cout<<"SiPMSD::ProcessHits_constStep()-->"<<aStep->GetPostStepPoint()->GetTouchable()->GetVolume()->GetName()
    //      <<" "<<aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber()<<G4endl;

    SiPMHit* hit = new SiPMHit(); //so create new hit
    hit->SetSiPMNumber(SipmNumber);
    hit->SetTime( aStep->GetPostStepPoint()->GetGlobalTime() );
    hit->SetPos( aStep->GetPostStepPoint()->GetPosition() );
    //hit->SetEventID(aStep->Ge)

    hitsCollection->insert(hit);

    //hit->IncPhotonCount(); //increment hit for the selected pmt

    return true;

}

void SiPMSD::EndOfEvent(G4HCofThisEvent *HCE){

    if (verboseLevel>1)
        PrintAll();
    G4int NbHits = hitsCollection->entries();
if(verboseLevel >0) {
    if (NbHits > 0) {
        G4cout << "Hit Number: " << NbHits << G4endl;
    }
}
}

void SiPMSD::clear() {
    G4VSensitiveDetector::clear();
}

void SiPMSD::DrawAll() {
    G4VSensitiveDetector::DrawAll();
}

void SiPMSD::PrintAll() {

        G4int NbHits = hitsCollection->entries();
    if(verboseLevel>1) {
        if (NbHits > 0) {
            G4cout << "\n-------->Hits Collection: in this event they are " << NbHits
                   << " hits in the SiPMs cathodes: " << G4endl;
            for (G4int i = 0; i < NbHits; i++) (*hitsCollection)[i]->Print();
        }
    }
}


