// automatically generated by the FlatBuffers compiler, do not modify

import * as flatbuffers from 'flatbuffers';

import { uCommand, unionTouCommand, unionListTouCommand } from '../../sensact/comm/u-command.js';


export class tCommand {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
  __init(i:number, bb:flatbuffers.ByteBuffer):tCommand {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

static getRootAstCommand(bb:flatbuffers.ByteBuffer, obj?:tCommand):tCommand {
  return (obj || new tCommand()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

static getSizePrefixedRootAstCommand(bb:flatbuffers.ByteBuffer, obj?:tCommand):tCommand {
  bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
  return (obj || new tCommand()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

static bufferHasIdentifier(bb:flatbuffers.ByteBuffer):boolean {
  return bb.__has_identifier('SCMD');
}

applicationId():number {
  const offset = this.bb!.__offset(this.bb_pos, 4);
  return offset ? this.bb!.readUint32(this.bb_pos + offset) : 0;
}

commandType():uCommand {
  const offset = this.bb!.__offset(this.bb_pos, 6);
  return offset ? this.bb!.readUint8(this.bb_pos + offset) : uCommand.NONE;
}

command<T extends flatbuffers.Table>(obj:any):any|null {
  const offset = this.bb!.__offset(this.bb_pos, 8);
  return offset ? this.bb!.__union(obj, this.bb_pos + offset) : null;
}

static starttCommand(builder:flatbuffers.Builder) {
  builder.startObject(3);
}

static addApplicationId(builder:flatbuffers.Builder, applicationId:number) {
  builder.addFieldInt32(0, applicationId, 0);
}

static addCommandType(builder:flatbuffers.Builder, commandType:uCommand) {
  builder.addFieldInt8(1, commandType, uCommand.NONE);
}

static addCommand(builder:flatbuffers.Builder, commandOffset:flatbuffers.Offset) {
  builder.addFieldOffset(2, commandOffset, 0);
}

static endtCommand(builder:flatbuffers.Builder):flatbuffers.Offset {
  const offset = builder.endObject();
  return offset;
}

static finishtCommandBuffer(builder:flatbuffers.Builder, offset:flatbuffers.Offset) {
  builder.finish(offset, 'SCMD');
}

static finishSizePrefixedtCommandBuffer(builder:flatbuffers.Builder, offset:flatbuffers.Offset) {
  builder.finish(offset, 'SCMD', true);
}

static createtCommand(builder:flatbuffers.Builder, applicationId:number, commandType:uCommand, commandOffset:flatbuffers.Offset):flatbuffers.Offset {
  tCommand.starttCommand(builder);
  tCommand.addApplicationId(builder, applicationId);
  tCommand.addCommandType(builder, commandType);
  tCommand.addCommand(builder, commandOffset);
  return tCommand.endtCommand(builder);
}
}