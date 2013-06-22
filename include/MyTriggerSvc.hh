//---------------------------------------------------------------------------//
//Description: Setup MyTrigger
//Author: Wu Chen(wuchen@mail.ihep.ac.cn)
//Created: 5 Jan, 2013
//Comment:
//---------------------------------------------------------------------------//

#ifndef MyTriggerSvc_h
#define MyTriggerSvc_h 1

#include "globals.hh"

class G4Event;
class MyDetectorManager;
class CdcLayerSD;
class CdcSD;
class MonitorSD;
class McTruthSvc;

class MyTriggerSvc
{
  public:
    MyTriggerSvc();
     ~MyTriggerSvc();

    static MyTriggerSvc* GetMyTriggerSvc();

    void SetMyTrigger(G4String name);

		bool TriggerIt( const G4Event*);

  private:

    static MyTriggerSvc* fMyTriggerSvc;

		void ReSet();

		void ShowOutCard();

		int minCdcHits;
		int minCdcCellHits;
		int minTriggerHits;
		double minEleMom;
		int minAntipNum;

		MyDetectorManager* pMyDetectorManager;
		CdcLayerSD* myCdcLayerSD;
		CdcSD* myCdcSD;
		MonitorSD* myTriggerSD;
		McTruthSvc* myMcTruthSvc;

};

#endif
