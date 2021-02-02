#pragma once

class CLC_Move {
private:
	char __PAD0[0x8]; // 0x0 two vtables
public:
	int m_nBackupCommands; // 0x8
	int m_nNewCommands;    // 0xC
	std::string* m_data;   // 0x10 std::string
	bf_read m_DataIn;      // 0x14
	bf_write m_DataOut;    // 0x38
};

class INetChannel;
class INetMessage
{
public:
	virtual	~INetMessage() {};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void	SetNetChannel( INetChannel * netchan ) = 0; // netchannel this message is from/for
	virtual void	SetReliable( bool state ) = 0;	// set to true if it's a reliable message

	virtual bool	Process( void ) = 0; // calles the recently set handler to process this message

	virtual	bool	ReadFromBuffer( bf_read &buffer ) = 0; // returns true if parsing was OK
	virtual	bool	WriteToBuffer( void* buffer ) = 0;	// returns true if writing was OK

	virtual bool	IsReliable( void ) const = 0;  // true, if message needs reliable handling

	virtual int				GetType( void ) const = 0; // returns module specific header tag eg svc_serverinfo
	virtual int				GetGroup( void ) const = 0;	// returns net message group of this message
	virtual const char		*GetName( void ) const = 0;	// returns network message name, eg "svc_serverinfo"
	virtual INetChannel		*GetNetChannel( void ) const = 0;
	virtual const char		*ToString( void ) const = 0; // returns a human readable string about message content
};

template < typename T >
class CNetMessagePB : public INetMessage, public T {};
using CCLCMsg_Move_t = CNetMessagePB< CLC_Move >;

class CClockDriftMgr
{
public:
	float m_ClockOffsets[ 17 ];   //0x0000
	uint32_t m_iCurClockOffset; //0x0044
	uint32_t m_nServerTick;     //0x0048
	uint32_t m_nClientTick;     //0x004C
}; //Size: 0x0050

class IClientState
{
public:
	void ForceFullUpdate()
	{
		m_nDeltaTick = -1;
	};

	std::byte pad_0[ 0x9c ];             //0x0000
	CNetChannel* m_NetChannel;          //0x009C
	uint32_t m_nChallengeNr;        //0x00A0
	std::byte pad_1[ 0x64 ];             //0x00A4
	uint32_t m_nSignonState;        //0x0108
	std::byte pad_2[ 0x8 ];               //0x010C
	float m_flNextCmdTime;          //0x0114
	uint32_t m_nServerCount;        //0x0118
	uint32_t m_nCurrentSequence;    //0x011C
	std::byte pad_3[ 0x4 ];               //0x0120
	CClockDriftMgr m_ClockDriftMgr; //0x0128
	uint32_t m_nDeltaTick;          //0x0178
	bool m_bPaused;                 //0x017C
	std::byte pad_4[ 0x7 ];               //0x017D
	uint32_t m_nViewEntity;         //0x0180
	uint32_t m_nPlayerSlot;         //0x0184
	char m_szLevelName[0x104];        //0x0188
	char m_szLevelNameShort[0x50];    //0x028C
	char m_szGroupName[0x50];         //0x02B4
	std::byte pad_5[0x5c];              //0x02DC
	uint32_t m_nMaxClients;         //0x0310
	std::byte pad_6[0x4984];           //0x0314
	float m_flLastServerTickTime;   //0x4C98
	bool insimulation;              //0x4C9C
	std::byte pad_7[0xb];               //0x4C9D
	uint32_t oldtickcount;          //0x4CA0
	float m_tickRemainder;          //0x4CA4
	float m_frameTime;              //0x4CA8
	uint32_t m_nLastOutgoingCommand;   //0x4CAC
	uint32_t m_nChokedCommands;        //0x4CB0
	uint32_t m_nLastCommandAck;      //0x4CB4
	uint32_t m_nCommandAck;           //0x4CB8
	uint32_t m_nSoundSequence;      //0x4CBC
	std::byte pad_8[0x50];              //0x4CC0
	Vector viewangles;              //0x4D10
	std::byte pad_9[ 0xd0 ];          //0x4D1C
	CEventInfo* m_Events;           //0x4DEC
}; 

extern IClientState* g_pClientState;