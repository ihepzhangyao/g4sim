//---------------------------------------------------------------------------//
//Description: Setup Material
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 17 Oct, 2012
//Comment:
//---------------------------------------------------------------------------//

#include "MaterialSvc.hh"

#include "G4Material.hh"
#include "G4UnitsTable.hh"
#include "globals.hh"

#include <sstream>
#include <fstream>
#include <string.h>
#include <iostream>

MaterialSvc* MaterialSvc::fMaterialSvc = 0;

MaterialSvc::MaterialSvc()
{
  if (fMaterialSvc){
    G4Exception("MaterialSvc::MaterialSvc()","Run0031",
        FatalException, "MaterialSvc constructed twice.");
  }
  fMaterialSvc = this;
}

MaterialSvc::~MaterialSvc()
{
  printf("~MaterialSvc\n");
}

MaterialSvc* MaterialSvc::GetMaterialSvc(){
  if ( !fMaterialSvc ){
    fMaterialSvc = new MaterialSvc;
  }
  return fMaterialSvc;
}

void MaterialSvc::SetMaterial( G4String file_name ){
	size_t sLast = file_name.last('/');
	if(sLast==std::string::npos){ // file name only
		G4String dir_name = getenv("CONFIGUREROOT");
		if (dir_name[dir_name.size()-1] != '/') dir_name.append("/");
		file_name = dir_name + file_name;
	}
  std::ifstream fin_card(file_name);
  if ( !fin_card ){
    std::cout<<"material list "<<file_name<<" is not available!!!"<<std::endl;
    G4Exception("MaterialSvc::SetMaterial()","Run0031",
        FatalException, "material list is not available.");
  }
  std::stringstream buf_card;
  std::string s_card;
  while(getline(fin_card,s_card)){
    buf_card.str("");
    buf_card.clear();
    buf_card<<s_card;
    const char* c = s_card.c_str();
    int length = strlen(c);
    int offset = 0;
    for ( ; offset < length; offset++ ){
      if ( c[offset] != ' ' ) break;
    }
    if ( c[offset] == '#' ) continue;
    else if ( c[offset] == '/' && c[offset+1] == '/' ) continue;
    else if ( length - offset == 0 ) continue;
    std::string keyword;
    buf_card>>keyword;
    if ( keyword == "TYPE" ){
      buf_card>>fMode;
      continue;
    }
    AddMaterial(s_card);
  }
  fin_card.close();
  buf_card.str("");
  buf_card.clear();
	// print table
	//
	G4cout << *(G4Element::GetElementTable()) << G4endl;
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

void MaterialSvc::AddMaterial( G4String content ){
  G4String symbol;
  G4String name;
  G4String s_temp;
  double z = 0;
  double a = 0;
  double density = 0;
  int ncomponents = 0;
  int natoms[50];
  double massFrac[50];
  double rel_dens = 0;
  G4String material[50];
  G4String element[50];
  std::stringstream buf_card;
  buf_card.str("");
  buf_card.clear();
  buf_card<<content;
  if ( fMode == "Elements"){
    buf_card>>symbol;
    buf_card>>name;
    buf_card>>z;
    buf_card>>a;
//    std::cout<<"symbol = "<<symbol<<", name = "<<name<<", a = "<<a<<", z = "<<z<<std::endl;
    new G4Element(name.c_str(), symbol.c_str(), z, a*g/mole );
  }
  else if ( fMode == "Simple_Materials" ){
    buf_card>>name;
    buf_card>>z;
    buf_card>>a;
    buf_card>>density;
    buf_card>>rel_dens;
//    std::cout<<"name = "<<name<<", a = "<<a<<", z = "<<z<<", density = "<<density<<", rel_dens = "<<rel_dens<<std::endl;
    new G4Material(name.c_str(), z, a*g/mole, density*rel_dens*g/cm3);
  }
  else if ( fMode == "Molecule_Materials" ){
    buf_card>>name;
    buf_card>>density;
    buf_card>>rel_dens;
    buf_card>>ncomponents;
    for ( int i = 0; i < ncomponents; i++ ){
      buf_card>>element[i];
      buf_card>>natoms[i];
    }
//    std::cout<<"name = "<<name<<", density = "<<density<<", rel_dens = "<<rel_dens<<", ncomponents = "<<ncomponents<<std::endl;
//    for ( int i = 0; i < ncomponents; i++ ){
//      std::cout<<"  "<<i<<": "<<"element = "<<element[i]<<", natoms = "<<natoms[i]<<std::endl;
//    }
    G4Material* new_card = new G4Material(name.c_str(), density*rel_dens*g/cm3, ncomponents);
    for ( int i = 0; i < ncomponents; i++ ){
      G4Element* new_ele = G4Element::GetElement(element[i]);
      new_card->AddElement(new_ele, natoms[i]);
    }
  }
  else if ( fMode == "MixEle_Materials" ){
    buf_card>>name;
    buf_card>>density;
    buf_card>>rel_dens;
    buf_card>>ncomponents;
    double sum_frac = 0;
    for ( int i = 0; i < ncomponents; i++ ){
      buf_card>>element[i];
      buf_card>>massFrac[i];
      sum_frac = sum_frac + massFrac[i];
    }
//    std::cout<<"name = "<<name<<", density = "<<density<<", rel_dens = "<<rel_dens<<", ncomponents = "<<ncomponents<<std::endl;
//    for ( int i = 0; i < ncomponents; i++ ){
//      std::cout<<"  "<<i<<": "<<"element = "<<element[i]<<", massFrac = "<<massFrac[i]<<std::endl;
//    }
    if ( sum_frac == 0 ){
      std::cout<<"Please check mass fractions for "<<name<<std::endl;
      G4Exception("MaterialSvc::AddMaterial()","Run0031",
          FatalException, "total fraction is zero.");
    }
    if ( sum_frac != 1 ){
      std::cout<<"the total fraction for "<<name<<" is not 1!!!"<<std::endl;
      std::cout<<"MaterialSvc will normalize it to 1!"<<std::endl;
      for ( int i = 0; i < ncomponents; i++ ){
        massFrac[i] = massFrac[i]/sum_frac;
      }
    }
    G4Material* new_card = new G4Material(name.c_str(), density*rel_dens*g/cm3, ncomponents);
    for ( int i = 0; i < ncomponents; i++ ){
      G4Element* new_ele = G4Element::GetElement(element[i]);
      new_card->AddElement(new_ele, massFrac[i]);
    }
  }
  else if ( fMode == "Mixture_Materials" ){
    buf_card>>name;
    buf_card>>rel_dens;
    buf_card>>ncomponents;
    double sum_frac = 0;
    for ( int i = 0; i < ncomponents; i++ ){
      buf_card>>material[i];
      buf_card>>massFrac[i];
      sum_frac += massFrac[i];
    }
//    std::cout<<"name = "<<name<<", rel_dens = "<<rel_dens<<", ncomponents = "<<ncomponents<<std::endl;
    if ( sum_frac == 0 ){
      std::cout<<"Please check mass fractions for "<<name<<std::endl;
      G4Exception("MaterialSvc::AddMaterial()","Run0031",
          FatalException, "total fraction is zero.");
    }
    if ( sum_frac != 1 ){
      std::cout<<"the total fraction for "<<name<<" is not 1!!!"<<std::endl;
      std::cout<<"MaterialSvc will normalize it to 1!"<<std::endl;
      for ( int i = 0; i < ncomponents; i++ ){
        massFrac[i] = massFrac[i]/sum_frac;
      }
    }
    for ( int i = 0; i < ncomponents; i++ ){
      G4Material* new_mat_com = G4Material::GetMaterial(material[i]);
      G4double i_density = new_mat_com->GetDensity();
//      std::cout<<"  "<<i<<": "<<"material = "<<material[i]<<", massFrac = "<<massFrac[i]<<", density = "<<(i_density/(g/cm3))<<std::endl;
      density += massFrac[i]/(i_density/(g/cm3));
    }
    if ( density!=0 ){
      density = 1/density;
    }
    G4Material* new_card = new G4Material(name.c_str(), density*rel_dens*g/cm3, ncomponents);
    for ( int i = 0; i < ncomponents; i++ ){
      G4Material* new_mat_com = G4Material::GetMaterial(material[i]);
      new_card->AddMaterial(new_mat_com, massFrac[i]);
    }
  }
  else{
    std::cout<<"mode "<<fMode<<" is not defined yet!!!"<<std::endl;
    G4Exception("MaterialSvc::AddMaterial()","Run0031",
        FatalException, "unknown mode type.");
  }
  buf_card.str("");
  buf_card.clear();
}