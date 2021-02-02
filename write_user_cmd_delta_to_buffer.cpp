#include "../include_cheat.h"
#include <intrin.h>

void write_user_cmd(bf_write* buf, CUserCmd* in, CUserCmd* out) {
	static DWORD write_user_cmd_f = (DWORD)sig("client.dll", "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D");

	__asm
	{
		mov     ecx, buf
		mov     edx, in
		push    out
		call    write_user_cmd_f
		add     esp, 4
	}
};

bool __fastcall hooks::write_user_cmd_delta_to_buffer(void* ecx_, void* edx_, int nslot, bf_write* buf, int from, int to, bool isnewcmd) {
	static auto sendmovecall = (void*)sig("engine.dll", "84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84");

	if (_ReturnAddress() != sendmovecall || g_tickbase_shift <= 0) {
		g_tickbase_shift = 0;
		return orig_write_user_cmd_delta_to_buffer(ecx_, edx_, nslot, buf, from, to, isnewcmd);
	}

	if (from != -1)
		return true;

	int m_nTickbase = g_tickbase_shift;
	g_tickbase_shift = 0;

	*(int*)((uintptr_t)buf - 0x30) = 0;
	int* m_pnNewCmds = (int*)((uintptr_t)buf - 0x2C);

	int m_nNewCmds = *m_pnNewCmds;
	int m_nNextCmd = g_pClientState->m_nLastOutgoingCommand + g_pClientState->m_nChokedCommands + 1;
	int m_nTotalNewCmds = std::min(m_nNewCmds + abs(m_nTickbase), 62);

	*m_pnNewCmds = m_nTotalNewCmds;

	for (to = m_nNextCmd - m_nNewCmds + 1; to <= m_nNextCmd; to++) {
		if (!orig_write_user_cmd_delta_to_buffer(ecx_, edx_, nslot, buf, from, to, true))
			return false;

		from = to;
	}

	CUserCmd* m_pCmd = g_pInput->GetUserCmd(nslot, from);
	if (!m_pCmd)
		return true;

	CUserCmd m_FromCmd = *m_pCmd, m_ToCmd = *m_pCmd;
	m_ToCmd.command_number++;
	m_ToCmd.tick_count += 3 * (int)std::round(1.f / g_pGlobals->interval_per_tick);

	for (int i = m_nNewCmds; i <= m_nTotalNewCmds; i++) {
		write_user_cmd(buf, &m_ToCmd, &m_FromCmd);
		m_FromCmd = m_ToCmd;

		m_ToCmd.command_number++;
		m_ToCmd.tick_count++;
	}

	return true;
}