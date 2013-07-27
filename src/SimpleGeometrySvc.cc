//---------------------------------------------------------------------------//
//Description: 1. Setup SimpleGeometry geometry and manage associated sensitive detectors
//             2. Communicate with associated sensitive detectors
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Modified: 11, Jan, 2013
//          Support G4Hype, G4TwistedTube
//Comment: about fVerboseLevel:
//         or modifying input card before starting a run.
//         Either way would set verbose of selected sensitive detectors to the same level
//         0:   Only Warnings and Errors
//         1:   Important information
//         >=5: All information
//---------------------------------------------------------------------------//
#include "SimpleGeometrySvc.hh"

#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4VSensitiveDetector.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Torus.hh"
#include "G4Hype.hh"
#include "G4TwistedTubs.hh"
#include "G4Cons.hh"
#include "G4Polycone.hh"
#include "G4Sphere.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4SolidStore.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4VisAttributes.hh"

#include <iostream>

#include "MyVGeometrySvc.hh"
#include "SimpleGeometryParameter.hh"
#include "MyDetectorManager.hh"

SimpleGeometrySvc::SimpleGeometrySvc(G4String name, G4String opt )
	: MyVGeometrySvc(name, "S") // Use a not-empty opt to stop inherited class from creating new GeometryParameter
{
	if ( opt == "" ){//called directly by user. set up parameter class.
		SimpleGeometryParameter *pointer = new SimpleGeometryParameter(name);
//		std::cout<<"======>In SimpleGeometrySvc, new SimpleGeometryParameter at ("<<(void*)pointer<<")!"<<std::endl;
		set_GeometryParameter(pointer);
	}
}

SimpleGeometrySvc::~SimpleGeometrySvc(){
	printf("~SimpleGeometrySvc");
}

//------------------------Modify-------------------------------
void SimpleGeometrySvc::set_GeometryParameter( SimpleGeometryParameter* val ){
	m_GeometryParameter = val;
	MyVGeometrySvc::set_GeometryParameter(m_GeometryParameter);
}

//--------------------------SetGeometry----------------------------------
//virtual function
G4VPhysicalVolume* SimpleGeometrySvc::SetGeometry(){
	m_GeometryParameter->Dump();
	ConstructVolumes();
	G4VPhysicalVolume *current = PlaceVolumes();
	return current;
}

//-------------------------Special functions-----------------------------
//=> ConstructVolumes
void SimpleGeometrySvc::ConstructVolumes(){
	MyVGeometrySvc::ConstructVolumes();
	bool vis;
	double r, g, b;
	G4Material* pttoMaterial;

	int nVol = m_GeometryParameter->get_VolNo();
	for ( int i_Vol = 0; i_Vol < nVol; i_Vol++ ){
		G4String name, mat_name, SDName; 
		G4String SolidType;
		G4int SolidIndex;
		SolidIndex = m_GeometryParameter->get_SolidIndex(i_Vol);
		SolidType = m_GeometryParameter->get_SolidType(i_Vol);
		name = m_GeometryParameter->get_name(i_Vol);
		G4VSolid* sol_Vol = 0;
		if ( SolidType == "Box" ){
			G4double halfX, halfY, halfZ;
			halfX = m_GeometryParameter->get_Box_X(SolidIndex)/2;
			halfY = m_GeometryParameter->get_Box_Y(SolidIndex)/2;
			halfZ = m_GeometryParameter->get_Box_Z(SolidIndex)/2;
			sol_Vol=new G4Box(name,halfX,halfY,halfZ);
		}
		else if ( SolidType == "Tubs" ){
			G4double RMax, RMin, halfLength, StartAng, SpanAng;
			RMax = m_GeometryParameter->get_Tubs_RMax(SolidIndex);
			RMin = m_GeometryParameter->get_Tubs_RMin(SolidIndex);
			halfLength = m_GeometryParameter->get_Tubs_Length(SolidIndex)/2;
			StartAng = m_GeometryParameter->get_Tubs_StartAng(SolidIndex);
			SpanAng = m_GeometryParameter->get_Tubs_SpanAng(SolidIndex);
			sol_Vol=new G4Tubs(name,RMin,RMax,halfLength,StartAng,SpanAng);
		}
		else if ( SolidType == "Torus" ){
			G4double RMax, RMin, Rtor, StartAng, SpanAng;
			RMax = m_GeometryParameter->get_Torus_RMax(SolidIndex);
			RMin = m_GeometryParameter->get_Torus_RMin(SolidIndex);
			Rtor = m_GeometryParameter->get_Torus_Rtor(SolidIndex);
			StartAng = m_GeometryParameter->get_Torus_StartAng(SolidIndex);
			SpanAng = m_GeometryParameter->get_Torus_SpanAng(SolidIndex);
			sol_Vol=new G4Torus(name,RMin,RMax,Rtor,StartAng,SpanAng);
		}
		else if ( SolidType == "Sphere" ){
			G4double RMax, RMin, StartPhi, SpanPhi, StartTheta, SpanTheta;
			RMax = m_GeometryParameter->get_Sphere_RMax(SolidIndex);
			RMin = m_GeometryParameter->get_Sphere_RMin(SolidIndex);
			StartPhi = m_GeometryParameter->get_Sphere_StartPhi(SolidIndex);
			SpanPhi = m_GeometryParameter->get_Sphere_SpanPhi(SolidIndex);
			StartTheta = m_GeometryParameter->get_Sphere_StartTheta(SolidIndex);
			SpanTheta = m_GeometryParameter->get_Sphere_SpanTheta(SolidIndex);
			sol_Vol=new G4Sphere(name,RMin,RMax,StartPhi,SpanPhi,StartTheta,SpanTheta);
		}
		else if ( SolidType == "Hype" ){
			G4double innerRadius, outerRadius, halfLength, innerStereo, outerStereo;
			innerRadius = m_GeometryParameter->get_Hype_innerRadius(SolidIndex);
			outerRadius = m_GeometryParameter->get_Hype_outerRadius(SolidIndex);
			halfLength = m_GeometryParameter->get_Hype_Length(SolidIndex)/2;
			innerStereo = m_GeometryParameter->get_Hype_innerStereo(SolidIndex);
			outerStereo = m_GeometryParameter->get_Hype_outerStereo(SolidIndex);
			sol_Vol=new G4Hype(name,innerRadius,outerRadius,innerStereo,outerStereo,halfLength);
		}
		else if ( SolidType == "TwistedTubs" ){
			G4double endinnerrad, endouterrad, halfLength, twistedangle, dphi;
			twistedangle = m_GeometryParameter->get_TwistedTubs_twistedangle(SolidIndex);
			endinnerrad = m_GeometryParameter->get_TwistedTubs_endinnerrad(SolidIndex);
			endouterrad = m_GeometryParameter->get_TwistedTubs_endouterrad(SolidIndex);
			halfLength = m_GeometryParameter->get_TwistedTubs_Length(SolidIndex)/2;
			dphi = m_GeometryParameter->get_TwistedTubs_dphi(SolidIndex);
			sol_Vol=new G4TwistedTubs(name,twistedangle,endinnerrad,endouterrad,halfLength,dphi);
		}
		else if ( SolidType == "Cons" ){
			G4double RMax1, RMin1, RMax2, RMin2, halfLength, StartAng, SpanAng;
			RMax1 = m_GeometryParameter->get_Cons_RMax1(SolidIndex);
			RMin1 = m_GeometryParameter->get_Cons_RMin1(SolidIndex);
			RMax2 = m_GeometryParameter->get_Cons_RMax2(SolidIndex);
			RMin2 = m_GeometryParameter->get_Cons_RMin2(SolidIndex);
			halfLength = m_GeometryParameter->get_Cons_Length(SolidIndex)/2;
			StartAng = m_GeometryParameter->get_Cons_StartAng(SolidIndex);
			SpanAng = m_GeometryParameter->get_Cons_SpanAng(SolidIndex);
			sol_Vol=new G4Cons(name,RMin1,RMax1,RMin2,RMax2,halfLength,StartAng,SpanAng);
		}
		else if ( SolidType == "Polycone" ){
			G4double StartAng, SpanAng;
			G4int numZ;
			numZ = m_GeometryParameter->get_Polycone_numZ(SolidIndex);
			G4double *RMax = new G4double[numZ];
			G4double *RMin = new G4double[numZ];
			G4double *Z = new G4double[numZ];
			for ( int i = 0; i < numZ; i++ ){
				RMax[i] = m_GeometryParameter->get_Polycone_RMax(SolidIndex,i);
				RMin[i] = m_GeometryParameter->get_Polycone_RMin(SolidIndex,i);
				Z[i] = m_GeometryParameter->get_Polycone_Z(SolidIndex,i);
			}
			StartAng = m_GeometryParameter->get_Polycone_StartAng(SolidIndex);
			SpanAng = m_GeometryParameter->get_Polycone_SpanAng(SolidIndex);
			sol_Vol=new G4Polycone(name,StartAng,SpanAng,numZ,Z,RMin,RMax);
		}
		else if ( SolidType == "BooleanSolid" ){
			G4double Ephi = m_GeometryParameter->get_BooleanSolid_Ephi(SolidIndex);
			G4double Etheta = m_GeometryParameter->get_BooleanSolid_Etheta(SolidIndex);
			G4double Epsi = m_GeometryParameter->get_BooleanSolid_Epsi(SolidIndex);
			G4double PosX  = m_GeometryParameter->get_BooleanSolid_PosX(SolidIndex);
			G4double PosY  = m_GeometryParameter->get_BooleanSolid_PosY(SolidIndex);
			G4double PosZ  = m_GeometryParameter->get_BooleanSolid_PosZ(SolidIndex);
			G4RotationMatrix* rot =new G4RotationMatrix(Ephi,Etheta,Epsi);
			G4ThreeVector trans(PosX ,PosY ,PosZ);
			G4String type = m_GeometryParameter->get_BooleanSolid_type(SolidIndex);
			G4String sol_name1 = m_GeometryParameter->get_BooleanSolid_sol1(SolidIndex);
			G4String sol_name2 = m_GeometryParameter->get_BooleanSolid_sol2(SolidIndex);
			G4SolidStore *pSolidStore = G4SolidStore::GetInstance();
			G4VSolid *sol1 = pSolidStore->GetSolid(sol_name1);
			G4VSolid *sol2 = pSolidStore->GetSolid(sol_name2);
			if (!sol1||!sol2){
				std::cout<<"ERROR: in SimpleGeometrySvc::ConstructVolumes(), cannot find solids for BooleanSolid \""<<name<<"\" !!!"<<std::endl;
				continue;
			}
			if ( type == "plus" ){
				sol_Vol = new G4UnionSolid(name,sol1,sol2,rot,trans);
			}
			else if ( type == "minus" ){
				sol_Vol = new G4SubtractionSolid(name,sol1,sol2,rot,trans);
			}
			else if ( type == "times" ){
				sol_Vol = new G4IntersectionSolid(name,sol1,sol2,rot,trans);
			}
			else {
				std::cout<<"ERROR: in SimpleGeometrySvc::ConstructVolumes(), cannot recognize BooleanSolid type \""<<type<<"\""<<"for \""<<name<<"\"!!!"<<std::endl;
				continue;
			}
		}
		else {
			std::cout<<"SolidType "<<SolidType<<" is not supported yet!"<<std::endl;
			G4Exception("SimpleGeometrySvc::ConstructVolumes()",
					"InvalidSetup", FatalException,
					"unknown SolidType");
		}
		if (m_GeometryParameter->get_SolidBoolean(i_Vol)) continue;
		SDName = m_GeometryParameter->get_SDName(i_Vol);
		mat_name = m_GeometryParameter->get_material(i_Vol);
		pttoMaterial = G4Material::GetMaterial(mat_name);
//		std::cout<<"name = "<<name
//		         <<", i_Vol = "<<i_Vol
//		         <<", SolidType = "<<SolidType
//		         <<", SolidIndex = "<<SolidIndex
//		         <<std::endl;
		if (!pttoMaterial){
			std::cout<<"Material "<<mat_name<<" is not defined!"<<std::endl;
			G4Exception("SimpleGeometrySvc::ConstructVolumes()",
					"InvalidSetup", FatalException,
					"unknown material name");
		}
		G4LogicalVolume* log_Vol;
		log_Vol = new G4LogicalVolume(sol_Vol, pttoMaterial, name,0,0,0);
		G4VSensitiveDetector* aSD = MyDetectorManager::GetMyDetectorManager()->GetSD(name, SDName, const_cast<SimpleGeometrySvc*>(this) );
		log_Vol->SetSensitiveDetector(aSD);
		//visual
		vis = m_GeometryParameter->get_vis(i_Vol);
		if (!vis){
			log_Vol->SetVisAttributes(G4VisAttributes::Invisible);
		}
		else{
			r = m_GeometryParameter->get_r(i_Vol);
			g = m_GeometryParameter->get_g(i_Vol);
			b = m_GeometryParameter->get_b(i_Vol);
			G4VisAttributes* visAttributes = new G4VisAttributes;
			visAttributes->SetVisibility(true);
			visAttributes->SetColour(r,g,b);
			log_Vol->SetVisAttributes(visAttributes);
		}
	}
}

//=>Place Volumes
G4VPhysicalVolume* SimpleGeometrySvc::PlaceVolumes(){
	G4VPhysicalVolume* world_pvol = 0;
	bool checkOverlap = m_GeometryParameter->get_checkoverlap();
	int nVol = m_GeometryParameter->get_VolNo();
	for ( int i_Vol = 0; i_Vol < nVol; i_Vol++ ){
		if (m_GeometryParameter->get_SolidBoolean(i_Vol)) continue;
		G4double PosX, PosY, PosZ;
		G4String name, motVolName; 
		G4double Space, DirTheta, DirPhi;
		G4int SRepNo;
		G4int RepNo;
		PosX  = m_GeometryParameter->get_PosX(i_Vol);
		PosY  = m_GeometryParameter->get_PosY(i_Vol);
		PosZ  = m_GeometryParameter->get_PosZ(i_Vol);
		name = m_GeometryParameter->get_name(i_Vol);
		motVolName = m_GeometryParameter->get_MotherName(i_Vol);
		SRepNo = m_GeometryParameter->get_SRepNo(i_Vol);
		RepNo = m_GeometryParameter->get_RepNo(i_Vol);
		Space = m_GeometryParameter->get_Space(i_Vol);
		DirTheta = m_GeometryParameter->get_DirTheta(i_Vol);
		DirPhi = m_GeometryParameter->get_DirPhi(i_Vol);
		G4LogicalVolume* log_Vol = get_logicalVolume(name);
		G4LogicalVolume* log_container;
		if ( name == "World" ) log_container = 0;
		else log_container = get_logicalVolume(motVolName);
		for ( int i = 0; i < RepNo; i++ ){
			G4ThreeVector pos(PosX ,PosY ,PosZ);
			G4ThreeVector dir(1, 0, 0);
			dir.setTheta(DirTheta);
			dir.setPhi(DirPhi);
			dir.setMag(Space);
			pos += dir * (2*i - RepNo + 1)/2.;
			G4double Ephi = m_GeometryParameter->get_Ephi(i_Vol);
			G4double Etheta = m_GeometryParameter->get_Etheta(i_Vol);
			G4double Epsi = m_GeometryParameter->get_Epsi(i_Vol);
			G4RotationMatrix* rotateMatrix=new G4RotationMatrix(Ephi,Etheta,Epsi);
			if ( name == "World" ) rotateMatrix = 0;
			G4VPhysicalVolume* phy_Vol =
				new G4PVPlacement(rotateMatrix,                //rotation
					pos,                              //position
					log_Vol,                          //its logical volume
					name,                             //its name
					log_container,                    //its mother volume
					false,                            //no boolean operation
					i+SRepNo,                                //copy number
					checkOverlap);                    //overlaps checking
			if ( name == "World" ) world_pvol = phy_Vol;
		}
	}
	G4VPhysicalVolume *former = MyVGeometrySvc::PlaceVolumes();
	if (!world_pvol) world_pvol=former;
	return world_pvol;
}
