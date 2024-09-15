

export interface AppManagement
{
    //TODO
};

export interface WidgetContainer{
    http_put_iocmd(buf:Uint8Array):void;
}

export interface ConfigWidgetManager{
    moveUp(i:number):void;
    moveDown(i:number):void;
    delete(i:number):void;
}