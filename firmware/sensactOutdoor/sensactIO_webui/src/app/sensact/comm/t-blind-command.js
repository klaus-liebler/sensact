"use strict";
// automatically generated by the FlatBuffers compiler, do not modify
Object.defineProperty(exports, "__esModule", { value: true });
exports.tBlindCommand = void 0;
var flatbuffers = require("flatbuffers");
var e_blind_command_js_1 = require("../../sensact/comm/e-blind-command.js");
var tBlindCommand = /** @class */ (function () {
    function tBlindCommand() {
        this.bb = null;
        this.bb_pos = 0;
    }
    tBlindCommand.prototype.__init = function (i, bb) {
        this.bb_pos = i;
        this.bb = bb;
        return this;
    };
    tBlindCommand.getRootAstBlindCommand = function (bb, obj) {
        return (obj || new tBlindCommand()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
    };
    tBlindCommand.getSizePrefixedRootAstBlindCommand = function (bb, obj) {
        bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
        return (obj || new tBlindCommand()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
    };
    tBlindCommand.prototype.cmd = function () {
        var offset = this.bb.__offset(this.bb_pos, 4);
        return offset ? this.bb.readInt8(this.bb_pos + offset) : e_blind_command_js_1.eBlindCommand.STOP;
    };
    tBlindCommand.starttBlindCommand = function (builder) {
        builder.startObject(1);
    };
    tBlindCommand.addCmd = function (builder, cmd) {
        builder.addFieldInt8(0, cmd, e_blind_command_js_1.eBlindCommand.STOP);
    };
    tBlindCommand.endtBlindCommand = function (builder) {
        var offset = builder.endObject();
        return offset;
    };
    tBlindCommand.createtBlindCommand = function (builder, cmd) {
        tBlindCommand.starttBlindCommand(builder);
        tBlindCommand.addCmd(builder, cmd);
        return tBlindCommand.endtBlindCommand(builder);
    };
    return tBlindCommand;
}());
exports.tBlindCommand = tBlindCommand;