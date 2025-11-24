//
// Created by liu on 18-10-23.
//

#ifndef SRC_DB_TABLE_H_
#define SRC_DB_TABLE_H_

#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <ostream>
#include <shared_mutex>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../utils/uexception.h"

class Table {
public:
  using KeyType = std::string;
  using FieldNameType = std::string;
  using FieldIndex = std::size_t;
  using ValueType = int;
  static constexpr const ValueType ValueTypeMax =
      std::numeric_limits<ValueType>::max();
  static constexpr const ValueType ValueTypeMin =
      std::numeric_limits<ValueType>::min();
  using SizeType = std::size_t;

private:
  class Datum {
  private:
    KeyType key_;
    std::vector<ValueType> datum_;

  public:
    Datum() = default;

    Datum(const Datum &) = default;

    auto operator=(const Datum &) -> Datum & = default;

    Datum(Datum &&) noexcept = default;

    auto operator=(Datum &&) noexcept -> Datum & = default;

    ~Datum() = default;

    explicit Datum(const SizeType &size)
        : datum_(std::vector<ValueType>(size, ValueType())) {}

    template <class ValueTypeContainer>
    explicit Datum(KeyType keyValue, ValueTypeContainer datumValue)
        : key_(std::move(keyValue)), datum_(std::move(datumValue)) {}

    // Take vector by value to ensure move; avoids clang-tidy
    // rvalue-ref-not-moved warning.
    explicit Datum(KeyType keyValue, std::vector<ValueType> datumValue)
        : key_(std::move(keyValue)), datum_(std::move(datumValue)) {}

    [[nodiscard]] auto key() const -> const KeyType & { return key_; }
    auto key() -> KeyType & { return key_; }

    [[nodiscard]] auto datum() const -> const std::vector<ValueType> & {
      return datum_;
    }
    auto datum() -> std::vector<ValueType> & { return datum_; }

    void setKey(KeyType keyValue) { key_ = std::move(keyValue); }
    void setDatum(std::vector<ValueType> datumValue) {
      datum_ = std::move(datumValue);
    }
  };

  mutable std::shared_mutex mutex_;

  using DataIterator = std::vector<Datum>::iterator;
  using ConstDataIterator = std::vector<Datum>::const_iterator;

  std::vector<FieldNameType> fields;
  std::unordered_map<FieldNameType, FieldIndex> fieldMap;
  std::vector<Datum> data;
  std::unordered_map<KeyType, SizeType> keyMap;
  std::string tableName;

  template <typename IteratorType, typename ValueType>
  static auto
  accessWithNameException(IteratorType iterator, const Table *table,
                          const FieldNameType &field) -> ValueType & {
    try {
      const auto &index = table->fieldMap.at(field);
      return iterator->datum().at(index);
    } catch (const std::out_of_range &exception) {
      throw TableFieldNotFound(R"(Field name "?" doesn't exists.)"_f % (field));
    }
  }

  template <typename IteratorType, typename ValueType>
  static auto accessWithIndexException(IteratorType iterator,
                                       const FieldIndex &index) -> ValueType & {
    try {
      return iterator->datum().at(index);
    } catch (const std::out_of_range &exception) {
      throw TableFieldNotFound(R"(Field index ? out of range.)"_f % (index));
    }
  }

public:
  using Ptr = std::unique_ptr<Table>;

  template <class Iterator, class VType> class ObjectImpl {
    friend class Table;
    Iterator iterator;
    using TablePtr =
        std::conditional_t<std::is_const_v<VType>, const Table *, Table *>;
    TablePtr table;

  public:
    using Ptr = std::unique_ptr<ObjectImpl>;
    using ValueTypeAlias = VType;  // expose value constness to IteratorImpl

    ObjectImpl(Iterator datumIterator, TablePtr tablePointer)
        : iterator(datumIterator), table(tablePointer) {}

    ObjectImpl(const ObjectImpl &) = default;

    ObjectImpl(ObjectImpl &&) noexcept = default;

    auto operator=(const ObjectImpl &) -> ObjectImpl & = default;

    auto operator=(ObjectImpl &&) noexcept -> ObjectImpl & = default;

    ~ObjectImpl() = default;

    [[nodiscard]] auto key() const -> KeyType { return iterator->key(); }

    // setKey available only when VType is non-const (C++20 concepts or
    // fallback)
#if defined(__cpp_concepts)
    template <typename T = VType>
    requires(!std::is_const_v<T>)
    void setKey(KeyType keyValue) {
      auto keyMapIterator = table->keyMap.find(iterator->key());
      auto dataIndex = keyMapIterator->second;
      table->keyMap.erase(keyMapIterator);
      table->keyMap.emplace(keyValue, dataIndex);
      iterator->setKey(std::move(keyValue));
    }
#else
    template <typename T = VType>
    std::enable_if_t<!std::is_const_v<T>, void> setKey(KeyType keyValue) {
      auto keyMapIterator = table->keyMap.find(iterator->key());
      auto dataIndex = keyMapIterator->second;
      table->keyMap.erase(keyMapIterator);
      table->keyMap.emplace(keyValue, dataIndex);
      iterator->setKey(std::move(keyValue));
    }
#endif

    auto operator[](const FieldNameType &field) const -> VType & {
      return Table::accessWithNameException<Iterator, VType>(iterator, table,
                                                             field);
    }

    auto operator[](const FieldIndex &index) const -> VType & {
      return Table::accessWithIndexException<Iterator, VType>(iterator, index);
    }

    [[nodiscard]] auto get(const FieldNameType &field) const -> VType & {
      return Table::accessWithNameException<Iterator, VType>(iterator, table,
                                                             field);
    }

    [[nodiscard]] auto get(const FieldIndex &index) const -> VType & {
      return Table::accessWithIndexException<Iterator, VType>(iterator, index);
    }
  };

  using Object = ObjectImpl<DataIterator, ValueType>;
  using ConstObject = ObjectImpl<ConstDataIterator, const ValueType>;

  template <typename ObjType, typename DatumIterator> class IteratorImpl {
    using difference_type = std::ptrdiff_t;
    using value_type = ObjType;
    using pointer = typename ObjType::Ptr;
    using reference = ObjType;
    using iterator_category = std::random_access_iterator_tag;

    friend class Table;

    DatumIterator iterator;
    using TablePtr2 =
        std::conditional_t<std::is_const_v<typename ObjType::ValueTypeAlias>,
                           const Table *, Table *>;
    TablePtr2 table = nullptr;

  public:
    IteratorImpl(DatumIterator datumIterator, TablePtr2 tablePointer)
        : iterator(datumIterator), table(tablePointer) {}

    IteratorImpl() = default;

    IteratorImpl(const IteratorImpl &) = default;

    IteratorImpl(IteratorImpl &&) noexcept = default;

    auto operator=(const IteratorImpl &) -> IteratorImpl & = default;

    auto operator=(IteratorImpl &&) noexcept -> IteratorImpl & = default;

    ~IteratorImpl() = default;

    auto operator->() -> pointer { return createProxy(iterator, table); }

    auto operator*() -> reference { return *createProxy(iterator, table); }

    auto operator+(int number) -> IteratorImpl {
      return IteratorImpl(iterator + number, table);
    }

    auto operator-(int number) -> IteratorImpl {
      return IteratorImpl(iterator - number, table);
    }

    auto operator+=(int number) -> IteratorImpl & {
      return iterator += number, *this;
    }

    auto operator-=(int number) -> IteratorImpl & {
      return iterator -= number, *this;
    }

    auto operator++() -> IteratorImpl & { return ++iterator, *this; }

    auto operator--() -> IteratorImpl & { return --iterator, *this; }

    auto operator++(int) -> IteratorImpl {
      auto returnValue = IteratorImpl(*this);
      ++iterator;
      return returnValue;
    }

    auto operator--(int) -> IteratorImpl {
      auto returnValue = IteratorImpl(*this);
      --iterator;
      return returnValue;
    }

    friend auto operator==(const IteratorImpl &lhs,
                           const IteratorImpl &rhs) -> bool {
      return lhs.iterator == rhs.iterator;
    }

    friend auto operator!=(const IteratorImpl &lhs,
                           const IteratorImpl &rhs) -> bool {
      return !(lhs == rhs);
    }

    auto operator<=(const IteratorImpl &other) -> bool {
      return this->iterator <= other.iterator;
    }

    auto operator>=(const IteratorImpl &other) -> bool {
      return this->iterator >= other.iterator;
    }

    auto operator<(const IteratorImpl &other) -> bool {
      return this->iterator < other.iterator;
    }

    auto operator>(const IteratorImpl &other) -> bool {
      return this->iterator > other.iterator;
    }
  };

  using Iterator = IteratorImpl<Object, decltype(data.begin())>;
  using ConstIterator = IteratorImpl<ConstObject, decltype(data.cbegin())>;

private:
  static auto createProxy(ConstDataIterator iterator,
                          const Table *tablePointer) -> ConstObject::Ptr {
    return std::make_unique<ConstObject>(iterator, tablePointer);
  }

  static auto createProxy(DataIterator iterator,
                          Table *tablePointer) -> Object::Ptr {
    return std::make_unique<Object>(iterator, tablePointer);
  }

public:
  Table() = delete;

  explicit Table(std::string name) : tableName(std::move(name)) {}

  template <class FieldIDContainer>
  Table(const std::string &name, const FieldIDContainer &fields);

  Table(std::string name, const Table &origin)
      : fields(origin.fields), fieldMap(origin.fieldMap), data(origin.data),
        keyMap(origin.keyMap), tableName(std::move(name)) {}

  [[nodiscard]] auto
  getFieldIndex(const FieldNameType &field) const -> FieldIndex;

  void insertByIndex(const KeyType &key, std::vector<ValueType> &&values);

  auto operator[](const KeyType &key) -> Object::Ptr;

  void setName(std::string name) { this->tableName = std::move(name); }

  [[nodiscard]] auto name() const -> const std::string & {
    return this->tableName;
  }

  [[nodiscard]] auto empty() const -> bool { return this->data.empty(); }

  [[nodiscard]] auto size() const -> std::size_t { return this->data.size(); }

  [[nodiscard]] auto field() const -> const std::vector<FieldNameType> & {
    return this->fields;
  }

  auto clear()
      -> std::
          size_t {  // NOLINT(readability-convert-member-functions-to-static)
    auto result = keyMap.size();
    data.clear();
    keyMap.clear();
    return result;
  }

  [[nodiscard]] auto begin() -> Iterator { return {data.begin(), this}; }

  [[nodiscard]] auto end() -> Iterator { return {data.end(), this}; }

  [[nodiscard]] auto begin() const -> ConstIterator {
    return {data.cbegin(), this};
  }

  [[nodiscard]] auto end() const -> ConstIterator {
    return {data.cend(), this};
  }

  auto erase(const KeyType &key) -> bool;

  /**
   * Erase multiple data rows by their keys. More efficient than calling
   * `erase` repeatedly when deleting many rows: this rebuilds the internal
   * storage once and updates the key map in linear time.
   * @param keys vector of keys to remove
   * @return number of rows actually removed
   */
  auto eraseMany(const std::vector<KeyType> &keys) -> SizeType;

  friend auto operator<<(std::ostream &outputStream,
                         const Table &table) -> std::ostream &;
};

auto operator<<(std::ostream &outputStream,
                const Table &table) -> std::ostream &;

template <class FieldIDContainer>
Table::Table(const std::string &name, const FieldIDContainer &fields)
    : fields(fields.cbegin(), fields.cend()), tableName(name) {
  SizeType index = 0;
  for (const auto &fieldName : fields) {
    if (fieldName ==
        "KEY") {  // Added braces per readability-braces-around-statements
      throw MultipleKey("Error creating table \"" + name +
                        "\": Multiple KEY field.");
    }
    fieldMap.emplace(fieldName, index++);
  }
}

#endif  // SRC_DB_TABLE_H_
