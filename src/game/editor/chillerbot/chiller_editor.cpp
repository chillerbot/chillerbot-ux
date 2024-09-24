#include <engine/shared/config.h>
#include <game/editor/editor.h>

void CEditor::BrushDrawChiller(const std::shared_ptr<CLayerTiles> &pLayer, const std::shared_ptr<CLayerTiles> &pBrush, vec2 WorldPos)
{
	if(!g_Config.m_ClMineTeeEditor)
		return;

	// assume that the brushed layer was the selected layer
	// if not we dont know which index the layer has
	if(!(int)m_vSelectedLayers.size())
		return;

	int Group = m_SelectedGroup;
	int Layer = m_vSelectedLayers[0];

	int sx = pLayer->ConvertX(WorldPos.x);
	int sy = pLayer->ConvertY(WorldPos.y);

	for(int y = 0; y < pBrush->m_Height; y++)
	{
		for(int x = 0; x < pBrush->m_Width; x++)
		{
			int fx = x + sx;
			int fy = y + sy;

			if(fx < 0 || fx >= pLayer->m_Width || fy < 0 || fy >= pLayer->m_Height)
				continue;

			CNetMsg_Sv_ModifyTile Msg;
			Msg.m_X = fx;
			Msg.m_Y = fy;
			Msg.m_Group = Group;
			Msg.m_Layer = Layer;
			Msg.m_Index = pBrush->GetTile(x, y).m_Index;
			Msg.m_Flags = pBrush->GetTile(x, y).m_Flags;
			CMsgPacker Packer(&Msg);
			Msg.Pack(&Packer);
			Client()->SendMsg(IClient::CONN_MAIN, &Packer, MSGFLAG_VITAL | MSGFLAG_FLUSH);
			// dbg_msg("ed", "x=%d y=%d", Msg.m_X, Msg.m_Y);
		}
	}
}
