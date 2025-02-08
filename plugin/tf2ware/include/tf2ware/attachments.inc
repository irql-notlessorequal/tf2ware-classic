/**************************************************************************
 *                                                                        *
 *                        Attachment Workaround                           *
 *                       Author: Mecha the Slag                           *
 *                           Version: 1.0.1                               *
 *                                                                        *
 **************************************************************************/

new Handle:attachments_array = INVALID_HANDLE;

/** 
 * Attaches a child to a parent.
 * 
 * @param child         Child entity. 
 * @param parent        Parent entity. 
 * @param variant       Attachment point. Empty for none. 
 * @return              True if able to create attachment. False if fail.
 *  
 * On error/Errors:     If unable to create attachment.
 */ 
stock CAttach(child, parent, const String:variant[]) {
    if (attachments_array == INVALID_HANDLE) attachments_array = CreateArray(2);
    if (!IsValidEntity(child)) return false;
    if (!IsValidEntity(parent)) return false;
    new link = CGetLink(child);
    if (link == -1 || !IsValidEntity(link)) link = CAddLink(child);
    if (link == -1 || !IsValidEntity(link)) {
        decl String:Classname[128];
        if (GetEdictClassname(child, Classname, sizeof(Classname))) ThrowError("Unable to create link for entity %s", Classname);
        else ThrowError("Unable to create link for unknown entity");
        return false;
    }
    
    new String:name[16];
    Format(name, sizeof(name), "target%i", parent);
    DispatchKeyValue(parent, "targetname", name);

    new String:name2[32];
    GetEntPropString(parent, Prop_Data, "m_iName", name2, sizeof(name2));
    DispatchKeyValue(link, "parentname", name2);
    
    
    SetVariantString(name2);
    AcceptEntityInput(link, "SetParent", link, link, 0);
    if (!StrEqual(variant, "")) {
        SetVariantString(variant);
        AcceptEntityInput(link, "SetParentAttachment", link, link, 0);
    }
    return true;
}

/** 
 * Detaches an entity from all its parents. Should be called before the entity is killed.
 * 
 * @param ent           Entity to detach. 
 * @return              True if an attachment is destroyed. False if no attachment.
 *  
 * On error/Errors:     Never.
 */ 
stock CDetach(ent) {
    if (attachments_array == INVALID_HANDLE) attachments_array = CreateArray(2);
    
    if (!IsValidEntity(ent)) return false;
    
    new link = CGetLink(ent);
    if (link != -1) {
        AcceptEntityInput(ent, "SetParent", -1, -1, 0);
        if (IsValidEntity(link) && link > 0) {
            decl String:strClassname[32];
            GetEdictClassname(link, strClassname, sizeof(strClassname));
            if (StrEqual(strClassname, "env_sprite"))
			{
                AcceptEntityInput(link, "kill");
            }
        }
        for (new i = 0; i < GetArraySize(attachments_array); i++) {
            new ent2 = GetArrayCell(attachments_array, i);
            if (ent == ent2) RemoveFromArray(attachments_array, i);
        }
        
        return true;
    }
    return false;
}

stock CGetLink(ent) {
    for (new i = 0; i < GetArraySize(attachments_array); i++) {
        new ent2 = GetArrayCell(attachments_array, i);
        if (ent == ent2) return (GetArrayCell(attachments_array, i, 1));
    }
    return -1;
}

stock CAddLink(ent) {
    new String:name_ent[16]; 
    Format(name_ent, sizeof(name_ent), "target%i", ent);
    DispatchKeyValue(ent, "targetname", name_ent);

    new link = CreateEntityByName("env_sprite");
    if (IsValidEntity(link)) {
        new Float:Origin[3];
        new Float:Rotation[3];
        GetEntPropVector(ent, Prop_Send, "m_vecOrigin", Origin);
        GetEntPropVector(ent, Prop_Send, "m_angRotation", Rotation);
    
        new String:name_link[16]; 
        Format(name_link, sizeof(name_link), "target%i", link);
        DispatchKeyValue(link, "targetname", name_link);
        
        DispatchKeyValue(link, "classname", "env_sprite");
        DispatchKeyValue(link, "spawnflags", "1");
        DispatchKeyValue(link, "scale", "0");
        DispatchKeyValue(link, "rendermode", "0");
        DispatchKeyValue(link, "rendercolor", "255 255 255");
        
        DispatchKeyValue(link, "model", "materials/sprites/minimap_icons/voiceicon.vmt");
        DispatchSpawn(link);
        TeleportEntity(link, Origin, Rotation, NULL_VECTOR);
        
        SetVariantString(name_link);
        AcceptEntityInput(ent, "SetParent", ent, ent, 0);
        
        SetVariantString("");
        AcceptEntityInput(link, "HideSprite");
        
        new index = PushArrayCell(attachments_array, ent);
        SetArrayCell(attachments_array, index, link, 1);
        return link;
    }
    return -1;
}