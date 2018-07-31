#ifndef ANVM_UTIL_ERROR_H_
#define ANVM_UTIL_ERROR_H_

#include <sstream>
#include <ostream>

class Error {
    public:
        Error() : exists_(false) { }

        Error(const Error& e) {
            if ((exists_ = e.exists_)) {
                ss_ << e.ss_.str();
            }
        }

        operator bool() const { return exists_; }

        bool operator==(const Error& e) {
            if (exists_ != e.exists_) {
                return false;
            }
            return ss_.str() == e.ss_.str();
        }

        friend std::ostream& operator<<(std::ostream& os, const Error& e);

        const std::string S() const { return ss_.str(); }

        template<typename T>
        Error& operator<<(const T& t) {
            exists_ = true;
            ss_ << t;
            return *this;
        }

        static const Error NONE;

    private:
        bool exists_;
        std::stringstream ss_;
};

#endif // ANVM_UTIL_ERROR_H_

