#pragma once
#include <inttypes.h>
#include <vector>
#include <spilcd16/common.hpp>

/*
Hierachie: Menu-Submenu-Menuitem
Jedes Submenu hat ein TopMenuItem und beliebig viele Menuitems

Annahme: Steuerung über 5way Switch
Up-Down wählt nächstes/vorheriges MenuItem aus
R-L wechselt zwischen den Submenüs, wenn der Cursor auf dem TopMenuItem steht
R-L wählt verschiedene Werte der MenuItems
OK speichert/übernimmt eine Änderung. Ansonsten wird die Änderung verworfen, wenn das Item verlassen wird
Alternative: OK wird nicht genutzt; Handbedienung ist aktiv, solange das entsprechende Submenu geöfffnet ist. Sobald auf
Alternative: Mit OK kann ein MenuItem freigeschaltet werden für Handbedienebene/I2C-Kontrolle

*/
class Menu{
public:

};

template<size_t Entries>
class Submenu{

};



class MenuContext{


};

enum class MenuReturnValue{
    QUIET,
    NEEDS_RERENDER,
};

class MenuItem{
public:
    //MenuItem muss in den buffer beginnend mit der offsetLine-te Zeile seine Darstellung schreiben
    //es darf davon ausgegangen werden. dass buf.len() ein ganzzahliges Vielfaches der Zeilenlänge ist
    //es darf davon ausgegangen werden, dass ein MenuItem die ganze Breite des Bildschirmes benötigt
    //es darf davon ausgegangen werden, dass ein MenuItem immer 20px hoch ist
    virtual MenuReturnValue RenderCallback(int16_t offsetLine, std::vector<uint8_t>* buf)=0;
    //Auf die folgenden Ereignisse kann ein MenuItem reagieren:
    //- Linker/Rechter Button geklickt
    //- wenn das Item den Fokus bekommt/verliert
    //- wenn das Management das Item zu einem Herholen seines aktuellen Wertes nötigen möchte (beim Öffnen des Submenüs und dann jede Sekunde einmal)
    virtual MenuReturnValue OnButtonLeft(MenuContext* ctx)=0;
    virtual MenuReturnValue OnButtonRight(MenuContext* ctx)=0;
    virtual MenuReturnValue OnButtonOK(MenuContext* ctx)=0;
    virtual MenuReturnValue OnFocusStart(MenuContext* ctx)=0;
    virtual MenuReturnValue OnFocusEnd(MenuContext* ctx)=0;
    virtual MenuReturnValue OnUpdateValue(MenuContext* ctx)=0;
};

//on-off
class MenuItemTwoWay:public MenuItem{
public:
    MenuItemTwoWay(uint8_t output):output(output){}

private:
    uint8_t output;
};

enum class ThreeWayMode:uint8_t{
    FIRST_ON_OFF_SECOND_DIRECTION,
};

//up-stop-down
class MenuItemThreeWay:public MenuItem{
public:
    MenuItemThreeWay(uint8_t output1, uint8_t output2, ThreeWayMode mode):output1(output1), output2(output2), mode(mode){}
private:
    uint8_t output1;
    uint8_t output2;
    ThreeWayMode mode;
};

//0-100%
class MenuItemPercent:public MenuItem{
public:
    MenuItemThreeWay(uint8_t output):output(output){}
private:
    uint8_t output;
};