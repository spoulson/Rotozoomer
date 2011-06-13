#pragma once

class CRotozoomer3Doc : public CDocument {
protected: // create from serialization only
   CRotozoomer3Doc();
   DECLARE_DYNCREATE(CRotozoomer3Doc)

public:
   virtual BOOL OnNewDocument();
   virtual void Serialize(CArchive &ar);
#ifdef SHARED_HANDLERS
   virtual void InitializeSearchContent();
   virtual void OnDrawThumbnail(CDC &dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

   virtual ~CRotozoomer3Doc();

#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext &dc) const;
#endif

protected:
   DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
   // Helper function that sets search content for a Search Handler
   void SetSearchContent(const CString &value);
#endif // SHARED_HANDLERS
};
