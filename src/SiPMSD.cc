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

    //User replica number 1 since photocathode is a daughter volume
    //to the pmt which was replicated
    G4int SipmNumber=
            aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber(1);
    G4VPhysicalVolume* physVol=
            aStep->GetPostStepPoint()->GetTouchable()->GetVolume(1);

    //Find the correct hit collection
    G4int n=hitsCollection->entries();
    SiPMHit* hit=NULL;
    for(G4int i=0;i<n;i++){
        if((*hitsCollection)[i]->GetSiPMNumber()==SipmNumber){
            hit=(*hitsCollection)[i];
            break;
        }
    }

    if(hit==NULL){//this pmt wasn't previously hit in this event
        hit = new SiPMHit(); //so create new hit
        hit->SetSiPMNumber(SipmNumber);
    //    hit->SetPMTPhysVol(physVol);
        hitsCollection->insert(hit);
    //    hit->SetPMTPos((*fPMTPositionsX)[pmtNumber],(*fPMTPositionsY)[pmtNumber],
    //                   (*fPMTPositionsZ)[pmtNumber]);
    }

    //hit->IncPhotonCount(); //increment hit for the selected pmt

}

void SiPMSD::EndOfEvent(G4HCofThisEvent *HCE){

    if (verboseLevel>0) {
        G4int NbHits = hitsCollection->entries();
        G4cout << "\n-------->Hits Collection: in this event they are " << NbHits
               << " hits in the readout pixel cathode: " << G4endl;
        for (G4int i=0;i<NbHits;i++) (*hitsCollection)[i]->Print();
        G4cout << "Hit Number: " << NbHits << G4endl;
    }


}

void SiPMSD::clear() {
    G4VSensitiveDetector::clear();
}

void SiPMSD::DrawAll() {
    G4VSensitiveDetector::DrawAll();
}

void SiPMSD::PrintAll() {

}


