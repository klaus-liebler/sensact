// automatically generated by the FlatBuffers compiler, do not modify

import * as flatbuffers from 'flatbuffers';

export class tRgbwPwmState {
  bb: flatbuffers.ByteBuffer|null = null;
  bb_pos = 0;
  __init(i:number, bb:flatbuffers.ByteBuffer):tRgbwPwmState {
  this.bb_pos = i;
  this.bb = bb;
  return this;
}

static getRootAstRgbwPwmState(bb:flatbuffers.ByteBuffer, obj?:tRgbwPwmState):tRgbwPwmState {
  return (obj || new tRgbwPwmState()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

static getSizePrefixedRootAstRgbwPwmState(bb:flatbuffers.ByteBuffer, obj?:tRgbwPwmState):tRgbwPwmState {
  bb.setPosition(bb.position() + flatbuffers.SIZE_PREFIX_LENGTH);
  return (obj || new tRgbwPwmState()).__init(bb.readInt32(bb.position()) + bb.position(), bb);
}

r():number {
  const offset = this.bb!.__offset(this.bb_pos, 4);
  return offset ? this.bb!.readUint8(this.bb_pos + offset) : 0;
}

g():number {
  const offset = this.bb!.__offset(this.bb_pos, 6);
  return offset ? this.bb!.readUint8(this.bb_pos + offset) : 0;
}

b():number {
  const offset = this.bb!.__offset(this.bb_pos, 8);
  return offset ? this.bb!.readUint8(this.bb_pos + offset) : 0;
}

ww():number {
  const offset = this.bb!.__offset(this.bb_pos, 10);
  return offset ? this.bb!.readFloat32(this.bb_pos + offset) : 0.0;
}

cw():number {
  const offset = this.bb!.__offset(this.bb_pos, 12);
  return offset ? this.bb!.readFloat32(this.bb_pos + offset) : 0.0;
}

on():boolean {
  const offset = this.bb!.__offset(this.bb_pos, 14);
  return offset ? !!this.bb!.readInt8(this.bb_pos + offset) : false;
}

static starttRgbwPwmState(builder:flatbuffers.Builder) {
  builder.startObject(6);
}

static addR(builder:flatbuffers.Builder, r:number) {
  builder.addFieldInt8(0, r, 0);
}

static addG(builder:flatbuffers.Builder, g:number) {
  builder.addFieldInt8(1, g, 0);
}

static addB(builder:flatbuffers.Builder, b:number) {
  builder.addFieldInt8(2, b, 0);
}

static addWw(builder:flatbuffers.Builder, ww:number) {
  builder.addFieldFloat32(3, ww, 0.0);
}

static addCw(builder:flatbuffers.Builder, cw:number) {
  builder.addFieldFloat32(4, cw, 0.0);
}

static addOn(builder:flatbuffers.Builder, on:boolean) {
  builder.addFieldInt8(5, +on, +false);
}

static endtRgbwPwmState(builder:flatbuffers.Builder):flatbuffers.Offset {
  const offset = builder.endObject();
  return offset;
}

static createtRgbwPwmState(builder:flatbuffers.Builder, r:number, g:number, b:number, ww:number, cw:number, on:boolean):flatbuffers.Offset {
  tRgbwPwmState.starttRgbwPwmState(builder);
  tRgbwPwmState.addR(builder, r);
  tRgbwPwmState.addG(builder, g);
  tRgbwPwmState.addB(builder, b);
  tRgbwPwmState.addWw(builder, ww);
  tRgbwPwmState.addCw(builder, cw);
  tRgbwPwmState.addOn(builder, on);
  return tRgbwPwmState.endtRgbwPwmState(builder);
}
}