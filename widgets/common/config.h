/**
* @File config.h
* @Date 2023-09-30
* @Description      
* @Author Ticks
* @Email ticks.cc\@gmail.com
* 
* Copyright (c) 2023 ${ORGANIZATION_NAME}. All rights reserved.
**/

#ifndef _QTFLUENTWIDGETS_CONFIG_H_
#define _QTFLUENTWIDGETS_CONFIG_H_

#include <string_view>
#include <vector>
#include <filesystem>
#include <memory>
#include <QObject>
#include <QColor>
#include <QVariant>
#include <fstream>
#include "magic_enum/magic_enum_all.hpp"
#include "json/nlohmann_json.h"

namespace QtFluentWidgets
{
	using Json = nlohmann::json;

    enum class Theme
    {
        Light = 1,
        Dark = 2,
        Auto = 3,
    }; // enum Theme

	template <typename Tp>
	struct IsEqComparable
	{
	 private:
		// 如果可比较，会选择这个重载版本
		template <typename T>
		static constexpr auto Check(T*) -> decltype(std::declval<T>() == std::declval<T>());
		// 低优先级函数
		template <typename T>
		static constexpr int Check(...);
	 public:
		static constexpr bool value = std::is_same_v<bool, decltype(Check<Tp>(nullptr))>;
	};

	template<typename Tp>
	struct IsComparable
	{
	 private:
		// std::declval 可以在不调用对象构造函数前提下构建一个对象引用, 如果可比较，会选择对应重载版本
		template <typename T>
		static constexpr auto Check(T*) -> decltype(std::declval<T>() == std::declval<T>());
		template <typename T>
		static constexpr auto Check(T*) -> decltype(std::declval<T>() < std::declval<T>());
		template <typename T>
		static constexpr auto Check(T*) -> decltype(std::declval<T>() <= std::declval<T>());
		template <typename T>
		static constexpr auto Check(T*) -> decltype(std::declval<T>() >= std::declval<T>());
		template <typename T>
		static constexpr auto Check(T*) -> decltype(std::declval<T>() > std::declval<T>());
		// 低优先级函数，都不能比较才会匹配
		template <typename T>
		static constexpr int Check(...);
	 public:
		static constexpr bool value = std::is_same_v<bool, decltype(Check<Tp>(nullptr))>;
	};

    template <typename Tp>
    struct ConfigValidatorBase
    {
		using Type = Tp;

        virtual bool validate(const Tp& val) const {
            return true;
        }

        virtual Tp correct(const Tp& val) const {
            return val;
        }

		virtual ~ConfigValidatorBase() = default;

    }; // struct ConfigValidatorBase<Tp, false>

    template <typename Tp, typename = std::enable_if_t<IsComparable<Tp>::value>>
    struct RangeValidator : public ConfigValidatorBase<Tp>
    {
    public:
        explicit RangeValidator(Tp _min, Tp _max)
            : min(std::min(_min, _max))
            , max(std::max(_min, _max))
        {  }
		~RangeValidator() override = default;

        bool validate(const Tp& val) const override {
            return min <= val && val <= max;
        }

        Tp correct(const Tp& val) const override {
            return std::min(std::max(min, val), max);
        }

        Tp min;
        Tp max;

    }; // struct RangeValidator

    template <typename Tp, typename = std::enable_if_t<IsEqComparable<Tp>::value>>
	struct OptionsValidator : public ConfigValidatorBase<Tp>
	{
		OptionsValidator(std::initializer_list<Tp> _opts)
			: opts(_opts)
		{  }
		template <typename First, typename End>
		OptionsValidator(First first, End end)
			: opts(first, end)
		{  }

		~OptionsValidator() override = default;

		bool validate(const Tp& val) const override {
			for (auto& it : opts) {
				if (it == val) {
					return true;
				}
			}
			return false;
		}

		Tp correct(const Tp& val) const override {
			if (this->validate(val)) {
				return val;
			}
			// opts不能为空！！
			return opts[0];
		}

		std::vector<Tp> opts;

	}; // struct OptionsValidator


    template <typename Ep, typename = std::enable_if_t<std::is_enum_v<Ep>>>
    struct EnumValidator : public ConfigValidatorBase<Ep>
    {
    public:
		EnumValidator()
		{
			// 根据枚举类型序列化枚举项
			auto names = magic_enum::enum_names<Ep>();
			opts.assign(names.begin(), names.end());
		}
		~EnumValidator() override = default;

        bool validate(const Ep& val) const override {
            return magic_enum::enum_contains<Ep>(val);
        }

        Ep correct(const Ep& val) const override {
            if (this->validate(val)) {
                return val;
            }
            return magic_enum::enum_value<Ep>(0);
        }

		std::vector<std::string> opts;

    }; // class EnumValidator

	struct FolderValidator
		: public ConfigValidatorBase<std::filesystem::path>
	{
	 public:
		~FolderValidator() override = default;

		bool validate(const std::filesystem::path& val) const override
		{
			return ::std::filesystem::exists(val);
		}
		std::filesystem::path correct(const std::filesystem::path& val) const override
		{
			::std::error_code code;
			::std::filesystem::create_directories(val, code);
			if (code) {
				return {
					::std::filesystem::absolute(val),
					::std::filesystem::path::generic_format
				};
			}
			return {""};
		}
	}; // class FolderValidator

	struct ColorValidator : public ConfigValidatorBase<QColor>
	{
	 public:
		~ColorValidator() override = default;

		bool validate(const QColor& val) const override
		{
			return val.isValid();
		}
		QColor correct(const QColor& val) const override
		{
			return val.isValid() ? val : QColor{};
		}

		QColor color{};

	}; // class ColorValidator

	struct ConfigItemQtSignalProxy : public QObject
	{
		Q_OBJECT
	 public:
		explicit ConfigItemQtSignalProxy(QObject* parent = nullptr)
			: QObject(parent)
		{  }

		signals:
		void valueChanged(QVariant);
	};

	template <typename Tp>
	struct ConfigItem : public ConfigItemQtSignalProxy
	{
//	 Q_PROPERTY(Tp value READ value WRITE setValue);
////	 Q_PROPERTY(std::string key READ key);

	 public:
		ConfigItem(std::string_view _group, std::string_view _name,
			const Tp& _default_val, ConfigValidatorBase<Tp>* validator,
			bool _restart = false)
			: group(_group)
			, name(_name)
			, default_val(_default_val)
			, _validator(validator)
			, restart(_restart)
		{  }

		~ConfigItem() override = default;

		Tp value() const
		{
			return default_val;
		}

		void setValue(const Tp& val)
		{
			if (_validator == nullptr) {
				return;
			}
			auto val1 = _validator->correct(val);
			auto ov = default_val;
			if (ov != val1) {
				default_val = val1;
				emit valueChanged(QVariant::fromValue(val1));
			}
		}
		std::string key() const
		{
			using namespace std::string_literals;
			return group + "."s + (name.empty() ? group : name);
		}

		std::string toStdString() const
		{
			std::stringstream ss;
			ss << this->metaObject()->className()
				<< "[value="
				<< default_val
				<< "]";
			return ss.str();
		}

		Json toJson() const
		{
			Json j;
			j["restart"] = restart;
			j["value"] = default_val;
			// validator
			return j;
		}

		std::string group;
		std::string name;
		bool restart {false};
		Tp default_val;
		std::unique_ptr<ConfigValidatorBase<Tp>> _validator {nullptr};

	}; // class ConfigItem

	class AppConfig : public QObject
	{
	 Q_OBJECT
	 signals:
		void appNeedRestart();
		void themeChanged(Theme);
		void themeChangeFinished();
		void themeColorChanged(QColor);

	 public:
		template <typename Tp>
		auto get(const ConfigItem<Tp>& item)
		{
			return item.value();
		}
		template <typename Tp>
		void set(const ConfigItem<Tp>& item, const Tp& val, bool save = true)
		{
			if (item.value() == val) {
				return;
			}
			item.setValue(val);
			if (save) {
				this->save();
			}
			if (item.restart()) {
				emit appNeedRestart();
			}
			if (item == this->_themeMode) {
				_theme = val;
				emit themeChanged(val);
			}
			if (item == this->_themeColor) {
				emit themeColorChanged(val);
			}
		}

		Json toJson() const
		{
			Json j;
			if (!_themeMode.name.empty()) {
				j[_themeMode.group][_themeMode.name] = _themeMode.toJson();
			}else {
				j[_themeMode.group].push_back(_themeMode.toJson());
			}
			if (!_themeColor.name.empty()) {
				j[_themeColor.group][_themeColor.name] = _themeColor.toJson();
			}else {
				j[_themeColor.group].push_back(_themeColor.toJson());
			}
			return j;
		}

		void save()
		{
			if (std::filesystem::create_directories(_path.parent_path())) {
				// 写入
				std::ofstream of(_path);
				of << this->toJson().dump(4) << "\n";
				of.flush();
			}
		}
		bool load(const std::filesystem::path& path)
		{
			if (!std::filesystem::exists(path)) {
				return false;
			}
			std::ifstream in(path);
			if (in) {
				try {
					Json j;
					in >> j;

				}catch (const Json::exception& exp) {
				}
			}
			return false;
		}

	 public:
		AppConfig()
			: _themeMode("QtFluentWidgets", "ThemeMode", Theme::Light, new EnumValidator<Theme>)
			, _themeColor("QtFluentWidgets", "ThemeColor", QColor(0, 159, 170), new ColorValidator)
			, _path("config/config.json")
			, _theme(Theme::Light)
		{  }

	 private:
		ConfigItem<Theme> _themeMode;
		ConfigItem<QColor> _themeColor;
		std::filesystem::path _path;
		Theme _theme;

	}; // class AppConfig


} // namespace QtFluentWidgets

Q_DECLARE_METATYPE(QtFluentWidgets::Theme)

namespace nlohmann
{
	template <>
	struct adl_serializer<QColor>
	{
		using Type = QColor;

		static void to_json(json& j, const Type& v)
		{
			j = v.name(QColor::HexArgb).toStdString();
		}
		static void from_json(const json& j, Type& v)
		{
			v.setNamedColor(j.get<std::string>().data());
		}
	};

	template <>
	struct adl_serializer<QtFluentWidgets::Theme>
	{
		using Type = QtFluentWidgets::Theme;

		static void to_json(json& j, const Type& v)
		{
			j = magic_enum::enum_name<Type>(v);
		}
		static void from_json(const json& j, Type& v)
		{
			auto vv = magic_enum::enum_cast<Type>(j.get<std::string>());
			if (vv.has_value()) {
				v = vv.value();
			}
		}
	};
}

#endif // _QTFLUENTWIDGETS_CONFIG_H_
