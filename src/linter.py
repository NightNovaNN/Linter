import ctypes
import os
import platform

# ==========================================================
# LOAD NATIVE LIB
# ==========================================================
def load_native_lib():
    system = platform.system()

    if system == "Windows":
        libname = "errorcheck.dll"
    elif system == "Linux":
        libname = "errorcheck.so"
    elif system == "Darwin":
        libname = "errorcheck.dylib"
    else:
        raise RuntimeError(f"Unsupported OS: {system}")

    here = os.path.dirname(os.path.abspath(__file__))
    path = os.path.join(here, libname)

    if not os.path.exists(path):
        raise FileNotFoundError(f"Native library not found: {path}")

    return ctypes.CDLL(path)


_native = load_native_lib()


# ==========================================================
# C STRUCT MAPPINGS
# ==========================================================
class LintError(ctypes.Structure):
    _fields_ = [
        ("line", ctypes.c_int),
        ("column", ctypes.c_int),
        ("message", ctypes.c_char * 256)
    ]


class ErrorList(ctypes.Structure):
    _fields_ = [
        ("items", ctypes.POINTER(LintError)),
        ("count", ctypes.c_int),
        ("capacity", ctypes.c_int),
    ]


# Tell Python about the function signature
_native.scan_all_errors.argtypes = [
    ctypes.c_char_p,
    ctypes.POINTER(ErrorList),
    ctypes.c_int
]
_native.scan_all_errors.restype = ctypes.c_int


# ==========================================================
# LANGUAGE MAP
# ==========================================================
LANG = {
    "c": 1,
    "cpp": 2,
    "java": 3,
    "python": 4
}


# ==========================================================
# PYTHON WRAPPER CLASS
# ==========================================================
class SmartLinter:
    def __init__(self, text: str):
        if isinstance(text, str):
            text = text.encode("utf-8")
        self.text = text

    def get_errors(self, lang: str):
        """Returns list of {line, column, message} errors"""

        lang_id = LANG.get(lang.lower())
        if lang_id is None:
            raise ValueError(f"Unknown language: {lang}")

        err_list = ErrorList()
        count = _native.scan_all_errors(self.text, ctypes.byref(err_list), lang_id)

        results = []
        for i in range(count):
            e = err_list.items[i]
            results.append({
                "line": e.line,
                "column": e.column,
                "message": e.message.decode("utf-8")
            })

        return results


# ==========================================================
# FILE LINTER
# ==========================================================
def lint_file(path, lang):
    with open(path, "r", encoding="utf-8") as f:
        code = f.read()
    return SmartLinter(code).get_errors(lang)


# ==========================================================
# MULTILANG TEST SUITE
# ==========================================================
if __name__ == "__main__":
    
    print("\n=== C TEST ===")
    code_c = """
    int main() {
        printf("hello);
        if (1 {
            return 0;
        }
    }
    """
    l = SmartLinter(code_c)
    for e in l.get_errors("c"):
        print(f"❌ {e['message']} @ line {e['line']} col {e['column']}")

    print("\n=== C++ TEST ===")
    code_cpp = """
    #include <iostream>
    using namespace std;

    int main() {
        cout << "Hello world;
        vector<int> v = {1, 2, 3;
        return 0;
    }
    """
    l = SmartLinter(code_cpp)
    for e in l.get_errors("cpp"):
        print(f"❌ {e['message']} @ line {e['line']} col {e['column']}")

    print("\n=== JAVA TEST ===")
    code_java = """
    class Test {
        public static void main(String[] args) {
            System.out.println("Hello);
            if (true {
                System.out.println("X");
            }
        }
    }
    """
    l = SmartLinter(code_java)
    for e in l.get_errors("java"):
        print(f"❌ {e['message']} @ line {e['line']} col {e['column']}")

    print("\n=== PYTHON TEST ===")
    code_py = '''
def test():
    msg = """Hello world
    print("oops)
      print("bad indent")
'''
    l = SmartLinter(code_py)
    for e in l.get_errors("python"):
        print(f"❌ {e['message']} @ line {e['line']} col {e['column']}")
