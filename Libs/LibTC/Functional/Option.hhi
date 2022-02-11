namespace TC::Functional {

template<typename T>
Option<T>::Option(T const& value)
    : m_is_present{ true } {
    new (&m_data_storage) T{ value };
}

template<typename T>
Option<T>::Option(T&& value)
    : m_is_present{ true } {
    new (&m_data_storage) T{ Cxx::move(value) };
}

template<typename T>
Option<T>::Option(Option const& rhs)
    : m_is_present{ rhs.m_is_present } {
    if ( m_is_present )
        new (&m_data_storage) T{ rhs.value() };
}

template<typename T>
Option<T>::Option(Option&& rhs) noexcept
    : m_is_present{ rhs.m_is_present } {
    if ( m_is_present )
        new (&m_data_storage) T{ Cxx::move(rhs.unwrap()) };
}

template<typename T>
Option<T>::~Option() {
    reset();
}

template<typename T>
Option<T>& Option<T>::operator=(T const& value) {
    reset();
    m_is_present = true;
    new (&m_data_storage) T{ value };
    return *this;
}

template<typename T>
Option<T>& Option<T>::operator=(T&& value) {
    reset();
    m_is_present = true;
    new (&m_data_storage) T{ Cxx::move(value) };
    return *this;
}

template<typename T>
Option<T>& Option<T>::operator=(Cxx::nullptr_t) {
    reset();
    return *this;
}

template<typename T>
Option<T>& Option<T>::operator=(Option const& rhs) {
    if ( this == &rhs )
        return *this;

    reset();
    m_is_present = rhs.m_is_present;
    new (&m_data_storage) T{ rhs.value() };
    return *this;
}

template<typename T>
Option<T>& Option<T>::operator=(Option&& rhs) noexcept {
    reset();
    m_is_present = Cxx::exchange(rhs.m_is_present, false);
    new (&m_data_storage) T{ Cxx::move(rhs.value()) };
    return *this;
}

template<typename T>
T& Option<T>::value() {
    VERIFY(is_present());

    return *__builtin_launder(reinterpret_cast<T*>(&m_data_storage));
}

template<typename T>
T const& Option<T>::value() const {
    VERIFY(is_present());

    return *__builtin_launder(reinterpret_cast<T const*>(&m_data_storage));
}

template<typename T>
T const& Option<T>::value_or(T const& default_value) const {
    if ( is_present() )
        return value();
    else
        return default_value;
}

template<typename T>
T Option<T>::unwrap() {
    T moved_value{ Cxx::move(value()) };
    reset();
    return moved_value;
}
template<typename T>
T Option<T>::unwrap_or(T const& default_value) {
    if ( is_present() )
        return unwrap();
    else
        return default_value;
}

template<typename T>
void Option<T>::reset() {
    if ( is_present() ) {
        value().~T();
        m_is_present = false;
    }
}

template<typename T>
bool Option<T>::is_present() const {
    return m_is_present;
}

template<typename T>
Option<T&>::Option(T const& value)
    : m_inner_option{ const_cast<T*>(&value) } {
}

template<typename T>
Option<T&>::Option(T&& value)
    : m_inner_option{ const_cast<T*>(&Cxx::move(value)) } {
}

template<typename T>
Option<T&>& Option<T&>::operator=(const T& value) {
    m_inner_option = &value;
    return *this;
}

template<typename T>
Option<T&>& Option<T&>::operator=(T&& value) {
    m_inner_option = &Cxx::move(value);
    return *this;
}

template<typename T>
Option<T&>& Option<T&>::operator=(Cxx::nullptr_t) {
    m_inner_option = nullptr;
    return *this;
}

template<typename T>
Option<T&>& Option<T&>::operator=(const Option& rhs) {
    m_inner_option = rhs.m_inner_option;
    return *this;
}

template<typename T>
Option<T&>& Option<T&>::operator=(Option&& rhs) noexcept {
    m_inner_option = Cxx::move(rhs.m_inner_option);
    return *this;
}

template<typename T>
T& Option<T&>::value() {
    return *m_inner_option.value();
}

template<typename T>
T const& Option<T&>::value() const {
    return *m_inner_option.value();
}

template<typename T>
T const& Option<T&>::value_or(T const& default_value) const {
    return *m_inner_option.value_or(default_value);
}

template<typename T>
T& Option<T&>::unwrap() {
    return *m_inner_option.unwrap();
}

template<typename T>
T& Option<T&>::unwrap_or(T const& default_value) {
    return *m_inner_option.unwrap_or(default_value);
}

template<typename T>
void Option<T&>::reset() {
    m_inner_option.reset();
}

template<typename T>
bool Option<T&>::is_present() const {
    return m_inner_option.is_present();
}

} /* namespace TC::Functional */