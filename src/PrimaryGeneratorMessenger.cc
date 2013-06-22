//---------------------------------------------------------------------------//
//Description: Messenger class
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithoutParameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(
                                          PrimaryGeneratorAction* Gun)
:Action(Gun)
{
  gunDir = new G4UIdirectory("/g4sim/gun/");
  gunDir->SetGuidance("PrimaryGenerator control");
   
  EnergyMode_cmd = new G4UIcmdWithAString("/g4sim/gun/EnergyMode",this);
  EnergyMode_cmd->SetGuidance("Choose energy model:");
  EnergyMode_cmd->SetGuidance("  Choice : none(default), histo");
  EnergyMode_cmd->SetParameterName("choice",true);
  EnergyMode_cmd->SetDefaultValue("none");
  EnergyMode_cmd->SetCandidates("none histo txt root");
  EnergyMode_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);
   
  DirectionMode_cmd = new G4UIcmdWithAString("/g4sim/gun/DirectionMode",this);
  DirectionMode_cmd->SetGuidance("Choose direction model:");
  DirectionMode_cmd->SetGuidance("  Choice : none(default), uniform");
  DirectionMode_cmd->SetParameterName("choice",true);
  DirectionMode_cmd->SetDefaultValue("none");
  DirectionMode_cmd->SetCandidates("none uniform");
  DirectionMode_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);
   
  PositionMode_cmd = new G4UIcmdWithAString("/g4sim/gun/PositionMode",this);
  PositionMode_cmd->SetGuidance("Choose position model:");
  PositionMode_cmd->SetGuidance("  Choice : none(default), uniform");
  PositionMode_cmd->SetParameterName("choice",true);
  PositionMode_cmd->SetDefaultValue("none");
  PositionMode_cmd->SetCandidates("none uniform");
  PositionMode_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  EM_hist_filename_cmd = new G4UIcmdWithAString("/g4sim/gun/EM_hist_filename",this);
  EM_hist_filename_cmd->SetGuidance("Choose name of input file for histogram to generate momentum amplitude:");
  EM_hist_filename_cmd->SetGuidance("  default name: default_file_for_mom");
  EM_hist_filename_cmd->SetParameterName("name",true);
  EM_hist_filename_cmd->SetDefaultValue("default_file_for_mom");
  EM_hist_filename_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  EM_hist_histname_cmd = new G4UIcmdWithAString("/g4sim/gun/EM_hist_histname",this);
  EM_hist_histname_cmd->SetGuidance("Choose name of input hist for histogram to generate momentum amplitude:");
  EM_hist_histname_cmd->SetGuidance("  default name: default_hist_for_mom");
  EM_hist_histname_cmd->SetParameterName("name",true);
  EM_hist_histname_cmd->SetDefaultValue("default_hist_for_mom");
  EM_hist_histname_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  root_filename_cmd = new G4UIcmdWithAString("/g4sim/gun/root_filename",this);
  root_filename_cmd->SetGuidance("root_filename");
  root_filename_cmd->SetParameterName("root_filename",false);
  root_filename_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  histo_build_cmd = new G4UIcmdWithoutParameter("/g4sim/gun/histo_build",this);
  histo_build_cmd->SetGuidance("update histogram for momentum.");
  histo_build_cmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  histo_build_cmd->SetGuidance("if you use energy model histo and changed input file.");
  histo_build_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  root_build_cmd = new G4UIcmdWithoutParameter("/g4sim/gun/root_build",this);
  root_build_cmd->SetGuidance("build TChain for root mode.");
  root_build_cmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  root_build_cmd->SetGuidance("if you use energy model root and changed input file.");
  root_build_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
  delete EnergyMode_cmd;
  delete DirectionMode_cmd;
  delete PositionMode_cmd;
  delete EM_hist_filename_cmd;
  delete EM_hist_histname_cmd;
  delete root_filename_cmd;
  delete histo_build_cmd;
  delete root_build_cmd;
  delete gunDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorMessenger::SetNewValue(
                                        G4UIcommand* command, G4String newValue)
{ 
  if( command == EnergyMode_cmd )    { Action->set_EnergyMode(newValue);}
  if( command == DirectionMode_cmd )    { Action->set_DirectionMode(newValue);}
  if( command == PositionMode_cmd )    { Action->set_PositionMode(newValue);}
  if( command == root_filename_cmd )    { Action->set_root_filename(newValue);}
  if( command == EM_hist_filename_cmd )    { Action->set_EM_hist_filename(newValue);}
  if( command == EM_hist_histname_cmd )    { Action->set_EM_hist_histname(newValue);}
  if( command == histo_build_cmd)    { Action->BuildHistoFromFile();}
  if( command == root_build_cmd)    { Action->root_build();}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
