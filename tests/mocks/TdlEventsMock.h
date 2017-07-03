//
// Created by mcochrane on 5/06/17.
//

#ifndef OSTBOARDFIRMWARE_TDLEVENTS_H
#define OSTBOARDFIRMWARE_TDLEVENTS_H

#include <gmock/gmock.h>
#include <TimerDescriptionLanguage/TdlEvent.h>

class TdlEvents {
public:
    TdlEvents() {};

//    void setCurrentEventAsProcessed() {
//        last_event_ = current_event_;
//        current_event_ = TdlEvent::Empty();
//    }
//
//    void setCurrentEvent(TdlEvent event) {
//        current_event_ = event;
//    }
//
//    TdlEvent& getLastEvent() { return last_event_; };
//    TdlEvent& getCurrentEvent() { return current_event_; };

    MOCK_METHOD0(setCurrentEventAsProcessed, void(void));
    MOCK_METHOD1(setCurrentEvent, void(TdlEvent));
    MOCK_METHOD0(getLastEvent, TdlEvent&(void));
    MOCK_METHOD0(getCurrentEvent, TdlEvent&(void));

};

TdlEvents& TdlEvents_GetInstance();

void TdlEvents_Init();
void TdlEvents_Destroy();


#endif //OSTBOARDFIRMWARE_TDLEVENTS_H
