class RemoteControl2Manager : public Rx470cCallback
{
    /*
    1:16579944
    2:16579940
    3:16579948
    4:16579938
    5:16579946
    6:16579942
    7:16579950
    8:16579937
    9:16579945
    10:16579941
    */
private:
    Manager *manager;
    std::map<uint32_t, std::function<flatbuffers::Offset<tCommand>(flatbuffers::FlatBufferBuilder *)>> keyCode2command{
        {16579944, [](flatbuffers::FlatBufferBuilder *builder)
         {
             auto xcmd = CreatetBlindCommand(*builder, eBlindCommand_DOWN_OR_STOP);
             return CreatetCommand(*builder, 1, uCommand::uCommand_tBlindCommand, xcmd.Union());
         }},
        {16579940, [](flatbuffers::FlatBufferBuilder *builder)
         {
             auto xcmd = CreatetBlindCommand(*builder, eBlindCommand_UP_OR_STOP);
             return CreatetCommand(*builder, 1, uCommand::uCommand_tBlindCommand, xcmd.Union());
         }},
        {16579948, [](flatbuffers::FlatBufferBuilder *builder)
         {
             auto xcmd = CreatetBlindCommand(*builder, eBlindCommand_DOWN_OR_STOP);
             return CreatetCommand(*builder, 2, uCommand::uCommand_tBlindCommand, xcmd.Union());
         }},
        {16579938, [](flatbuffers::FlatBufferBuilder *builder)
         {
             auto xcmd = CreatetBlindCommand(*builder, eBlindCommand_UP_OR_STOP);
             return CreatetCommand(*builder, 2, uCommand::uCommand_tBlindCommand, xcmd.Union());
         }},
        {16579946, [](flatbuffers::FlatBufferBuilder *builder)
         {
             auto xcmd = CreatetSinglePwmCommand(*builder, eSinglePwmCommand_TOGGLE);
             return CreatetCommand(*builder, 3, uCommand::uCommand_tSinglePwmCommand, xcmd.Union());
         }},
    };

public:
    void ReceivedFromRx470c(uint32_t val)
    {
        flatbuffers::FlatBufferBuilder builder(128);
        flatbuffers::Offset<tCommand> cmd;
        if (keyCode2command.find(val) == keyCode2command.end())
        {
            ESP_LOGW(TAG, "KeyCode %d has no associated handler", val);
            return;
        }
        cmd = (keyCode2command[val])(&builder);

        builder.Finish(cmd);
        auto parsed_cmd = flatbuffers::GetRoot<tCommand>(builder.GetBufferPointer());
        manager->HandleCommand(parsed_cmd);
    }
    RemoteControl2Manager(Manager *manager) : manager(manager) {}
};