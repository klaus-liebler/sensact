"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.SerializeContext = void 0;
class SerializeContext {
    constructor(buffer, bufferOffset = 0) {
        this.buffer = buffer;
        this.bufferOffset = bufferOffset;
        this.bufferDV = new DataView(buffer);
    }
    writeGUID(guid) {
        for (let i = 0; i < 16; i++) {
            this.bufferDV.setUint8(this.bufferOffset + i, guid[i]);
        }
        this.bufferOffset += 16;
    }
    consumeGUIDandCompare(compare) {
        let val = true;
        for (let i = 0; i < 16; i++) {
            if (compare[i] != this.bufferDV.getUint8(this.bufferOffset + i)) {
                val = false;
                break;
            }
        }
        this.bufferOffset += 16;
        return val;
    }
    funhash(start, end) {
        end = Math.min(end, this.bufferDV.byteLength);
        for (var i = start, h = 0xdeadbeef; i < end; i++)
            h = Math.imul(h ^ this.bufferDV.getUint8(i), 2654435761);
        return (h ^ h >>> 16) >>> 0;
    }
    overwriteU32(theNumber, offset) {
        if (offset > this.bufferOffset - 4)
            throw new Error("offset>this.bufferOffset-4");
        this.bufferDV.setUint32(offset, theNumber, true);
    }
    writeS32(theNumber) {
        this.bufferDV.setInt32(this.bufferOffset, theNumber, true);
        this.bufferOffset += 4;
    }
    writeU32(theNumber) {
        this.bufferDV.setUint32(this.bufferOffset, theNumber, true);
        this.bufferOffset += 4;
    }
    writeF32(theNumber) {
        this.bufferDV.setFloat32(this.bufferOffset, theNumber, true);
        this.bufferOffset += 4;
    }
    readF32() {
        let val = this.bufferDV.getFloat32(this.bufferOffset, true);
        this.bufferOffset += 4;
        return val;
    }
    readU32() {
        let val = this.bufferDV.getUint32(this.bufferOffset, true);
        this.bufferOffset += 4;
        return val;
    }
    readS32() {
        let val = this.bufferDV.getInt32(this.bufferOffset, true);
        this.bufferOffset += 4;
        return val;
    }
    getResult() {
        return this.buffer.slice(0, this.bufferOffset);
    }
}
exports.SerializeContext = SerializeContext;
//# sourceMappingURL=SerializeContext.js.map