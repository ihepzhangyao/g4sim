//---------------------------------------------------------------------------//
//Description: Setup PrimaryGenerator
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include <map>

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class PrimaryGeneratorMessenger;
class TChain;
class TFile;
class TH1F;
class G4ParticleDefinition;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
	public:
		PrimaryGeneratorAction();    
		virtual ~PrimaryGeneratorAction();

		void GeneratePrimaries(G4Event*);

		void ReadCard(G4String);

		void set_EnergyMode(G4String val) { EnergyMode= val; }
		void set_DirectionMode(G4String val) { DirectionMode= val; }
		void set_PositionMode(G4String val) { PositionMode= val; }
		void set_TimeMode(G4String val) { TimeMode= val; }
		void set_pidMode(G4String val) { pidMode= val; }
		void set_EM_hist_filename(G4String val) { EM_hist_filename= val; }
		void set_EM_hist_histname(G4String val) { EM_hist_histname= val; }
		void set_DM_hist_filename(G4String val) { DM_hist_filename= val; }
		void set_DM_hist_histname(G4String val) { DM_hist_histname= val; }
		void set_root_filename(G4String val) { root_filename= val; }
		void set_root_treename(G4String val) { root_treename= val; }
		void set_root_index(int val){root_index=val;}


		void SetUniformDirection();
		void SetRandomPosition();
		void SetRandomEnergy();
		void SetUniformPosition();
		void BuildHistoFromFile();
		void root_get_para();
		void root_build();
		void root_set_Position();
		void root_set_Energy();
		void root_set_Time();
		void root_set_pid();

		//  typedef std::map<G4int, class G4ParticleDefinition *> ParticleCache;

		//protected:
		//  ParticleCache targetMap;

	private:
		//	G4double get_mom_from_histo();
		void Dump();

		G4ParticleGun*             particleGun;	 //pointer a to G4  class
		PrimaryGeneratorMessenger* gunMessenger;   //messenger of this class
		G4ParticleDefinition*      fParticle;

		//general options
		G4String                   fType;
		G4String                   ParticleName;
		G4int                      Z,A,C;
		G4double                   E;
		G4double                   Pa;
		G4double                   Ekin;
		G4double                   mass;
		G4int                      EnergyType;
		G4double                   Px,Py,Pz;
		G4double                   Theta,Phi;
		G4double                   x,y,z,t;
		G4double                   xSpread,ySpread,zSpread;
		G4double                   MomSpread;
		G4double                   EkinSpread;
		G4String                   EnergyMode;
		G4String                   PositionMode;
		G4String                   TimeMode;
		G4String                   pidMode;
		G4String                   DirectionMode;

		//For EnergyMode
		TH1F*                      EM_hist;
		G4String                   EM_hist_filename;
		G4String                   EM_hist_histname;
		TH1F*                      DM_hist;
		G4String                   DM_hist_filename;
		G4String                   DM_hist_histname;

		//For PositionMode
		G4String                   UP_SubDet;
		G4String                   UP_Volume;
		G4String                   UP_Type;
		//For DirectionMode

		//For root file
		G4bool                     UseRoot;
		TChain*                    m_TChain;
		G4int                      root_num;
		G4int                      root_index;
		G4String                   root_filename;
		G4String                   root_treename;
		G4double                   root_double[7];
		G4int                      root_int[1];
		TFile                      *fp;
};

#endif

