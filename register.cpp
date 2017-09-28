//class CObject;
struct CRuntimeClass {
    char *m_lpszClassName;
    int m_nObjectSize;
    void* (*m_pfnCreateObject)();
    CRuntimeClass* m_pNextClass;
    static CRuntimeClass* pFirstClass;
    void* CreateObject(const char* lpszClassName);
};
CRuntimeClass* CRuntimeClass::pFirstClass = NULL;

void* CRuntimeClass::CreateObject(const char* lpszClassName) {
    CRuntimeClass* ptr = pFirstClass;
    while (ptr != NULL && strcmp(ptr->m_lpszClassName, lpszClassName) != 0) {
        ptr = ptr->m_pNextClass;
    }

    if (ptr == NULL || ptr->m_pfnCreateObject == NULL) {
        return NULL;
    }

    void* pObject = NULL;
    try {
        pObject = (*(ptr->m_pfnCreateObject))();
    } catch(exception) {
        return NULL;
    }

    return pObject;
}

#define DECLARE_DYNAMIC(class_name)\
public:\
static CRuntimeClass class##class_name;\
virtual CRuntimeClass* GetRuntimeClass() const;

#define IMPLEMENT_DYNAMIC(class_name)\
char _lpsz##class_name[]=#class_name;\
void* Create##class_name()\
{ return new class_name; }\
CRuntimeClass class_name::class##class_name={\
_lpsz##class_name,sizeof(class_name),Create##class_name};\
static CLASS_INIT _init_##class_name(&class_name::class##class_name);\
CRuntimeClass* class_name::GetRuntimeClass() const\
{ return &class_name::class##class_name; }

struct CLASS_INIT {
    CLASS_INIT(CRuntimeClass* pNewClass);
};

CLASS_INIT::CLASS_INIT(CRuntimeClass* pNewClass) {
    pNewClass->m_pNextClass = CRuntimeClass::pFirstClass;
    CRuntimeClass::pFirstClass = pNewClass;
}

class Base {
DECLARE_DYNAMIC(Base)

public:
    virtual void parse() {
        cout << "Base" << endl;
    }
    virtual ~Base() {}
};

IMPLEMENT_DYNAMIC(Base)

class A: public Base {
DECLARE_DYNAMIC(A)
public:
    virtual void parse() {
        cout << "A" << endl;
    }
    virtual ~A() {}
};

IMPLEMENT_DYNAMIC(A)

#define CREATE_USERSELF_OBJECT(class_name, object) \
(static_cast<class_name*>(object))
