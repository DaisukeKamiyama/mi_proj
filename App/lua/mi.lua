-- Libraries for Lua (v0.01)

function InputText(viewID,str)
  local range = GetSelectedRange(viewID)
  local docID = GetDocumentIDFromViewID(viewID)
  DeleteText(docID,range.s,range.e)
  InsertText(docID,range.s,str)
  local p = range.s+string.len(str);
  SetSelectedRange(viewID,p,p)
end

function GetSelectedText(viewID)
  local range = GetSelectedRange(viewID)
  local docID = GetDocumentIDFromViewID(viewID)
  return GetText(docID,range.s,range.e)
end

function GetParagraphRange(docID,paraIndex)
  local range = {}
  range.s = GetParagraphStartTextIndex(docID,paraIndex)
  range.e = GetParagraphStartTextIndex(docID,paraIndex+1)
  return range
end

function GetParagraphText(docID,paraIndex)
  local range = GetParagraphRange(docID,paraIndex)
  return GetText(docID,range.s,range.e)
end

