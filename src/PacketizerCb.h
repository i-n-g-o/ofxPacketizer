//
//  PacketizerCb.h
//  r2000Poco
//
//  Created by inx on 17/03/16.
//
//

#ifndef PacketizerCb_h
#define PacketizerCb_h

class PacketizerCb
{
public:
	virtual void onPacketStart() {};
	virtual void onPacket(uint8_t*, size_t) {};
	virtual void onPacketOverflow(uint8_t*, size_t) {};
};


#endif /* PacketizerCb_h */
