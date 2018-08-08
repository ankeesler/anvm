#include "linker.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <ostream>
#include <vector>
#include <map>

#include "symbol_table.h"
#include "src/program.h"
#include "src/util/log.h"
#include "src/util/error.h"

#define LOG(...) log_->Printf("linker", __FILE__, __LINE__, __VA_ARGS__)

static Error ResolveSymbols(const SymbolTable& st,
        std::map<std::string, const Symbol*> *resolved,
        std::map<Word, std::string> *unresolved);

Error Linker::Link(const SymbolTable& st, Program *program) {
    LOG("Starting linker");
    std::map<std::string, const Symbol*> resolvedSymbols;
    std::map<Word, std::string> unresolvedSymbols;
    Error error = ResolveSymbols(st, &resolvedSymbols, &unresolvedSymbols);
    if (error) {
        LOG("Failed to resolve symbols: %s", error.S().c_str());
        return error;
    }

    for (const std::pair<std::string, const Symbol *>& kv : resolvedSymbols) {
        const std::string& name = kv.first;
        const Symbol *symbol = resolvedSymbols[name];
        program->AddWords(symbol->address, symbol->words);
        LOG("Added symbol %s at address 0x%08X (length = %d)", name.c_str(), symbol->address, symbol->words.size());
    }

    for (const std::pair<Word, std::string>& kv : unresolvedSymbols) {
        Word address = kv.first;
        const std::string& name = kv.second;
        const Symbol *symbol = resolvedSymbols[name];
        program->AddWord(address, symbol->address);
        LOG("Resolving symbol %s at address 0x%08X", name.c_str(), address);
    }

    LOG("Returning from linker");
    return Error::NONE;
}

static Error ResolveSymbols(const SymbolTable& st,
        std::map<std::string, const Symbol*> *resolved,
        std::map<Word, std::string> *unresolved) {
    Error e;

    const std::vector<std::string>& names = st.SymbolNames();
    for (const std::string& name : names) {
        std::vector<Symbol*> symbols = st.Symbols(name);
        std::vector<Word> unresolvedAddresses;
        for (const Symbol *symbol : symbols) {
            if (symbol->resolved) {
                if (resolved->count(name) > 0) {
                    e << "Duplicate symbol " << name << " at address " << symbol->address << "\n";
                } else {
                    resolved->insert(std::pair<std::string, const Symbol*>(name, symbol));
                }
            } else {
                unresolvedAddresses.push_back(symbol->address);
            }
        }

        if (resolved->count(name) == 0) {
            e << "Unresolved symbol " << name << " at addresses ";
            for (Word address : unresolvedAddresses) {
                char hex[9]; // 0x000000 + '\0'
                memset(hex, 0, sizeof(hex));
                sprintf(hex, "0x%08X", address);
                e << hex << " ";
            }
            e << "\n";
        } else {
            for (Word address : unresolvedAddresses) {
                unresolved->insert(std::pair<Word, std::string>(address, name));
            }
        }
    }

    return e;
}
