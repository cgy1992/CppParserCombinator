// ----------------------------------------------------------------------------
// Copyright 2015 Mårten Rånge
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------------------------------------------------------
#include "stdafx.h"
// ----------------------------------------------------------------------------
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <tuple>
// ----------------------------------------------------------------------------
#include "cpp_pc/pc.hpp"
// ----------------------------------------------------------------------------
#define TEST_EQ(expected, actual) test_eq (__FILE__, __LINE__, __FUNCTION__, #expected, expected, #actual, actual)
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
namespace test_parser
{
  using namespace cpp_pc;

  std::ostream & operator << (std::ostream & o, unit_type const &)
  {
    o
      << "unit"
      ;
    return o;
  }

  namespace detail
  {
    template<std::size_t i, std::size_t sz>
    struct print_tuple
    {
      template<typename... TArgs>
      static void print (std::ostream & o, std::tuple<TArgs...> const & v)
      {
        o
          << ", "
          << std::get<i> (v)
          ;

        print_tuple<i + 1, sz>::print (o, v);
      }
    };

    template<std::size_t sz>
    struct print_tuple<sz, sz>
    {
      template<typename... TArgs>
      static void print (std::ostream &, std::tuple<TArgs...> const &)
      {
      }
    };
  }

  std::ostream & operator << (std::ostream & o, std::tuple<> const &)
  {
    o
      << "()"
      ;
    return o;
  }

  template<typename... TArgs>
  std::ostream & operator << (std::ostream & o, std::tuple<TArgs...> const & v)
  {
    o
      << "("
      << std::get<0> (v)
      ;

    detail::print_tuple<1, sizeof... (TArgs)>::print (o, v);

    o
      << ")"
      ;
    return o;
  }

  template<typename TValue>
  std::ostream & operator << (std::ostream & o, std::vector<TValue> const & vs)
  {
    o
      << "[("
      << vs.size ()
      << ")"
      ;

    for (auto && v : vs)
    {
      o
        << ", "
        << v
        ;
    }

    o
      << "]"
      ;

    return o;
  }

  template<typename TValue>
  std::ostream & operator << (std::ostream & o, opt<TValue> const & v)
  {
    if (v)
    {
      o
        << "Some "
        << v.get ()
        ;
    }
    else
    {
      o
        << "None"
        ;
    }
    return o;
  }

  template<typename TValue>
  std::ostream & operator << (std::ostream & o, result<TValue> const & v)
  {
    o
      << "{result: "
      << v.position
      << ", "
      << v.value
      << "}"
      ;
    return o;
  }

  template<typename TExpected, typename TActual>
  bool test_eq (
      char const *      file
    , int               line_no
    , char const *      // func
    , char const *      expected_name
    , TExpected const & expected
    , char const *      actual_name
    , TActual const &   actual
    )
  {
    if (!(expected == actual))
    {
      std::cout
        << file << "(" << line_no << "): EQ - "
        << expected_name << "(" << expected << ")"
        << " == "
        << actual_name << "(" << actual << ")"
        << std::endl
        ;
      return false;
    }

    return true;
  }

  template<typename T>
  void test_opt (T const & one, T const & /*two*/)
  {
    {
      opt<T> empty;
      TEST_EQ (true, empty.is_empty ());
    }


    {
      opt<T> empty = empty_opt;
      TEST_EQ (true, empty.is_empty ());
    }

    {
      opt<T> o { one };
      if (TEST_EQ (false, o.is_empty ()))
      {
        TEST_EQ (one, o.get ());
      }
    }

    {
      opt<T> o { T (one) };
      if (TEST_EQ (false, o.is_empty ()))
      {
        TEST_EQ (one, o.get ());
      }
    }

    {
      opt<T> o;
      opt<T> c = o;
      TEST_EQ (true, o.is_empty ());
      TEST_EQ (true, c.is_empty ());
      TEST_EQ (true, o == c);
    }

    {
      opt<T> o;
      opt<T> c = std::move (o);
      TEST_EQ (true, o.is_empty ());
      TEST_EQ (true, c.is_empty ());
      TEST_EQ (true, o == c);
    }

    {
      opt<T> o;
      opt<T> c;
      c = o;
      TEST_EQ (true, o.is_empty ());
      TEST_EQ (true, c.is_empty ());
      TEST_EQ (true, o == c);
    }

    {
      opt<T> o;
      opt<T> c;
      c = std::move (o);
      TEST_EQ (true, o.is_empty ());
      TEST_EQ (true, c.is_empty ());
      TEST_EQ (true, o == c);
    }

    {
      opt<T> o { one };
      opt<T> c = o;
      if (TEST_EQ (false, o.is_empty ()))
      {
        TEST_EQ (one, o.get ());
      }
      if (TEST_EQ (false, c.is_empty ()))
      {
        TEST_EQ (one, c.get ());
      }
      TEST_EQ (true, o == c);
    }

    {
      opt<T> o { one };
      opt<T> c = std::move (o);
      TEST_EQ (true, o.is_empty ());
      if (TEST_EQ (false, c.is_empty ()))
      {
        TEST_EQ (one, c.get ());
      }
      TEST_EQ (true, o != c);
    }

    {
      opt<T> o { one };
      opt<T> c;
      c = o;
      if (TEST_EQ (false, o.is_empty ()))
      {
        TEST_EQ (one, o.get ());
      }
      if (TEST_EQ (false, c.is_empty ()))
      {
        TEST_EQ (one, c.get ());
      }
      TEST_EQ (true, o == c);
    }

    {
      opt<T> o { one };
      opt<T> c;
      c = std::move (o);
      TEST_EQ (true, o.is_empty ());
      if (TEST_EQ (false, c.is_empty ()))
      {
        TEST_EQ (one, c.get ());
      }
      TEST_EQ (true, o != c);
    }
  }

  void test_parser ()
  {
    std::string input = "1234 + 5678";

    {
      auto p =
            preturn (3)
        ;
      result<int> expected  = success (0, 3);
      result<int> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            preturn (3)
        >=  [] (int v) { return preturn (std::make_tuple (v, 4)); }
        ;
      result<std::tuple<int,int>> expected  = success (0, std::make_tuple (3, 4));
      result<std::tuple<int,int>> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            psatisfy ("digits", 1U, 10U, satisfy_digit)
        >=  [] (auto && v) { return preturn (v.str ()); }
        ;
      result<std::string> expected  = success (4, std::string ("1234"));
      result<std::string> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_char ('1')
        ;
      result<unit_type> expected  = success (1, unit);
      result<unit_type> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_char ('2')
        ;
      result<unit_type> expected  = failure<unit_type> (0);
      result<unit_type> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_char ('1')
        >   pskip_char ('2')
        ;
      result<unit_type> expected  = success (2, unit);
      result<unit_type> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_char ('1')
        >   pskip_char ('1')
        ;
      result<unit_type> expected  = failure<unit_type> (1);
      result<unit_type> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_char ('1')
        <   pskip_char ('2')
        ;
      result<unit_type> expected  = success (2, unit);
      result<unit_type> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_char ('1')
        <   pskip_char ('1')
        ;
      result<unit_type> expected  = failure<unit_type> (1);
      result<unit_type> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    // Extend pint test to test +/-
    {
      auto p =
            pint
        ;
      result<int> expected  = success (4, 1234);
      result<int> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_ws
        ;
      result<unit_type> expected  = success (0, unit);
      result<unit_type> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_string ("123")
        ;
      result<unit_type> expected  = success (3, unit);
      result<unit_type> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_string ("124")
        ;
      result<unit_type> expected  = failure<unit_type> (2);
      result<unit_type> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_string ("123")
        <   preturn (true)
        ;
      result<bool> expected  = success (3, true);
      result<bool> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_string ("124")
        <   preturn (true)
        ;
      result<bool> expected  = failure<bool> (2);
      result<bool> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_char ('1')
        ;
      result<unit_type> expected  = success (1, unit);
      result<unit_type> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pskip_char ('2')
        ;
      result<unit_type> expected  = failure<unit_type> (0);
      result<unit_type> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pmany (1, 5, pany_of ("123"))
        ;
      result<std::vector<char>> expected  = success (3, std::vector<char> ({'1','2','3'}));
      result<std::vector<char>> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pmany (1, 5, pany_of ("456"))
        ;
      result<std::vector<char>> expected  = failure<std::vector<char>> (0);
      result<std::vector<char>> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pmany (1, 2, pany_of ("123"))
        ;
      result<std::vector<char>> expected  = success (2, std::vector<char> ({'1','2'}));
      result<std::vector<char>> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pmany (5, 5, pany_of ("123"))
        ;
      result<std::vector<char>> expected  = failure<std::vector<char>> (0);
      result<std::vector<char>> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            ptuple (pskip_char ('1'), pskip_char ('2') < preturn (1))
        ;
      result<std::tuple<unit_type, int>> expected = success (2, std::make_tuple (unit, 1));
      result<std::tuple<unit_type, int>> actual   = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            ptuple (pskip_char ('2'), pskip_char ('1') < preturn (1))
        ;
      result<std::tuple<unit_type, int>> expected = failure<std::tuple<unit_type, int>> (0);
      result<std::tuple<unit_type, int>> actual   = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            ptuple (pskip_char ('1'), pskip_char ('1') < preturn (1))
        ;
      result<std::tuple<unit_type, int>> expected = failure<std::tuple<unit_type, int>> (1);
      result<std::tuple<unit_type, int>> actual   = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            popt (pskip_char ('1'))
        ;
      result<opt<unit_type>> expected  = success (1, make_opt (unit));
      result<opt<unit_type>> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            popt (pskip_char ('2'))
        ;
      result<opt<unit_type>> expected  = success (0, opt<unit_type> ());
      result<opt<unit_type>> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pint
        >   pskip_ws
        >   pskip_char ('+')
        >   pskip_ws
        >=  [] (int v) { return pint >= [v] (int u) { return preturn (std::make_tuple (v,u)); }; }
        ;
      result<std::tuple<int,int>> expected  = success (11, std::make_tuple (1234, 5678));
      result<std::tuple<int,int>> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    {
      auto p =
            pint
        >   pskip_ws
        >   pskip_char ('-')
        >   pskip_ws
        >=  [] (int v) { return pint >= [v] (int u) { return preturn (std::make_tuple (v,u)); }; }
        ;
      result<std::tuple<int,int>> expected  = failure<std::tuple<int,int>> (5);
      result<std::tuple<int,int>> actual    = plain_parse (p, input);
      TEST_EQ (expected, actual);
    }

    // TODO:
    // punit
    // pchoice
    // ptrampoline
    // pbreakpoint
    // pbetween
    // psep
    // peos
    // pskip_satisfy
    // psatisfy_char
  }
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
namespace calculator
{
  using namespace cpp_pc;

  using variables = std::map<std::string, int>;

  struct expr
  {
    using ptr = std::shared_ptr<expr>;

    expr ()                         = default;
    expr (expr const &)             = delete ;
    expr (expr &&)                  = delete ;
    expr & operator = (expr const &)= delete ;
    expr & operator = (expr &&)     = delete ;
    virtual ~expr ()                = default;

    virtual void  build_string (std::ostream & o) const = 0;
    virtual int   eval (variables const & vs) const     = 0;
  };

  struct int_expr : expr
  {
    int value;

    int_expr (int v)
      : value (std::move (v))
    {
    }

    void build_string (std::ostream & o) const override
    {
      o << value;
    }

    int eval (variables const &) const override
    {
      return value;
    }

    static expr::ptr create (int v)
    {
      return std::make_shared<int_expr> (v);
    }
  };

  struct identifier_expr : expr
  {
    std::string id;

    identifier_expr (std::string id)
      : id (std::move (id))
    {
    }

    void build_string (std::ostream & o) const override
    {
      o << id;
    }

    int eval (variables const & vs) const override
    {
      auto find = vs.find (id);
      if (find != vs.end ())
      {
        return find->second;
      }
      else
      {
        // TODO: Error handler
        return 0;
      }
    }

    static expr::ptr create (sub_string const & s)
    {
      return std::make_shared<identifier_expr> (s.str ());
    }
  };

  struct binary_expr : expr
  {
    expr::ptr left  ;
    char      op    ;
    expr::ptr right ;

    binary_expr (expr::ptr left, char op, expr::ptr right)
      : left  (std::move (left))
      , op    (std::move (op))
      , right (std::move (right))
    {
    }

    void build_string (std::ostream & o) const override
    {
      o
        << '('
        ;

      left->build_string (o);

      o
        << ' '
        << op
        << ' '
        ;

      right->build_string (o);

      o
        << ')'
        ;
    }

    int eval (variables const & vs) const override
    {
      auto l = left->eval (vs);
      auto r = right->eval (vs);
      switch (op)
      {
      default:
        // Error handling
        return 0;
      case '+':
        return l + r;
      case '-':
        return l - r;
      case '*':
        return l * r;
      case '/':
        return l / r;
      case '%':
        return l % r;
      }
    }

    static expr::ptr create (expr::ptr left, char op, expr::ptr right)
    {
      return std::make_shared<binary_expr> (std::move (left), std::move (op), std::move (right));
    }
  };

  auto satisfy_identifier = [] (std::size_t pos, char ch)
    {
      return
            (ch >= 'A' && ch <= 'Z'               )
        ||  (ch >= 'a' && ch <= 'z'               )
        ||  ((pos > 0) && ch >= '0' && ch <= '9'  )
        ;
    };
  auto pidentifier_expr = pmap (psatisfy ("identifier", 1, SIZE_MAX, satisfy_identifier), identifier_expr::create);

  auto pint_expr        = pmap (pint, int_expr::create);

  auto pexpr_trampoline = create_trampoline<expr::ptr> ();
  auto pexpr            = ptrampoline<expr::ptr> (pexpr_trampoline);
  auto psub_expr        = pbetween (pskip_char ('(') > pskip_ws, pexpr, pskip_char (')'));

  auto pvalue_expr      = pchoice (pint_expr, pidentifier_expr, psub_expr) > pskip_ws;

  auto p0_op =
        pany_of ("*/%")
    >   pskip_ws
    ;
  auto pop0_expr        = psep (pvalue_expr , p0_op , binary_expr::create);

  auto p1_op =
        pany_of ("+-")
    >   pskip_ws
    ;
  auto pop1_expr        = psep (pop0_expr   , p1_op , binary_expr::create);

  auto pcalculator_expr = [] ()
  {
    pexpr_trampoline->trampoline = pop1_expr.parser_function;
    return pskip_ws < pexpr > peos;
  } ();

  variables const vars
  {
    {"x"  , 3},
    {"y"  , 5},
  };

  void parse_and_print (const std::string & input)
  {
    auto r = parse (pcalculator_expr, input);
    if (r.value)
    {
      auto expr = r.value.get ();
      auto v = expr->eval (vars);

      std::stringstream o;
      expr->build_string (o);

      std::cout
        << "Parsed: " << input << std::endl
        << "  as  : " << o.str () << std::endl
        << "  eval: " << v << std::endl
        ;
    }
    else
    {
      std::cout << r.message << std::endl;
    }
  }

  void test_calculator ()
  {
    std::cout << "Variables:" << std::endl;
    for (auto && kv : vars)
    {
      std::cout << "  " << kv.first << " = " << kv.second << std::endl;
    }
    parse_and_print ("1234");
    parse_and_print ("abc");
    parse_and_print ("(0 + 3) * x + 4*y - 1");

    std::string line;
    while (std::cout << "Input expression (blank to exit)" << std::endl, std::getline (std::cin, line) && !line.empty ())
    {
      parse_and_print (line);
    }

  }

}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
namespace json
{
  using namespace cpp_pc;

  struct json_ast
  {
    using ptr = std::shared_ptr<json_ast> ;
    json_ast ()                             = default;
    json_ast (json_ast const &)             = delete ;
    json_ast (json_ast &&)                  = delete ;
    json_ast & operator = (json_ast const &)= delete ;
    json_ast & operator = (json_ast &&)     = delete ;
    virtual ~json_ast ()                    = default;
  };

  struct json_boolean : json_ast
  {
    bool value;
  };

  struct null_type
  {
    constexpr null_type ()
    {
    }
  };

  constexpr null_type null_value;

  auto satisfy_char = [] (std::size_t, char ch)
    {
      return ch != '"' && ch != '\\';
    };
  auto map_escaped = [] (char ch)
    {
      switch (ch)
      {
      case '"':
        return '"';
      case '\\':
        return '\\';
      case '/':
        return '/';
      case 'b':
        return '\b';
      case 'f':
        return '\f';
      case 'n':
        return '\n';
      case 'r':
        return '\r';
      case 't':
        return '\t';
      default:
        CPP_PC__ASSERT (false);
        return ch;
      };
    };

  auto map_number = [] (auto && v)
    {
      auto calculate_fraction = [] (auto && frac)
        {
          auto i = static_cast<double> (std::get<0> (frac));
          auto s = std::get<1> (frac);
          return i / std::pow (10.0, s);
        };

      auto calculate_exponent = [] (auto && exp)
        {
          auto sign   = (std::get<0> (exp).coalesce ('+') == '+') ? 1.0 : -1.0;
          auto e      = std::get<1> (exp);
          return std::pow (10.0, sign*e);
        };

      auto sign = std::get<0> (v) ? -1.0 : 1.0;
      auto i    = static_cast<double> (std::get<1> (v));
      auto ofrac= std::get<2> (v);
      auto frac =
          ofrac
        ? calculate_fraction (ofrac.get ())
        : 0.0
        ;
      auto oexp = std::get<3> (v);
      auto exp =
          oexp
        ? calculate_exponent (oexp.get ())
        : 1.0
        ;
      return sign * (i + frac) * exp;
    };

  auto pchar    = psatisfy_char ("char", satisfy_char);
  auto pescaped = pskip_char ('\\') < pmap (pany_of ("\"\\/bfnrt"), map_escaped);
  auto pchars   = pmany (0, SIZE_MAX, pchoice (pchar, pescaped));
  auto pstring  = pbetween (pskip_char ('"'), pchars, pskip_char ('"'));

  auto pfrac    = popt (pskip_char ('.') < praw_uint64);
  auto psign    = popt (pany_of ("+-"));
  auto pexp     = popt (pany_of ("eE") < ptuple (psign, pint));
  auto pnumber  = pmap (ptuple (popt (pskip_char ('-')), pint64, pfrac, pexp), map_number);

  auto ptrue    = pskip_string ("true")   < preturn (true);

  auto pfalse   = pskip_string ("false")  < preturn (false);

  auto pnull    = pskip_string ("null")   < preturn (null_value);

  auto pvalue   = pnull > pskip_ws;
  auto pvalues  = pmany (0, SIZE_MAX, pvalue > pskip_char (',') > pskip_ws);

  auto parray   = pbetween (pskip_char ('[') > pskip_ws, pvalues, pskip_char (']') > pskip_ws);

  auto pmember  = ptuple (pstring > pskip_ws > pskip_char (':') > pskip_ws, pvalue);
  auto pmembers = pmany (0, SIZE_MAX, pmember > pskip_char (',') > pskip_ws);
  auto pobject  = pbetween (pskip_char ('{') > pskip_ws, pmembers, pskip_char ('}') > pskip_ws);

//  auto pjson    = pskip_ws > pchoice (parray, pobject) > peos;

  void parse_and_print (const std::string & input)
  {
    auto r = parse (pnumber > peos, input);
    if (r.value)
    {

      double v = r.value.get ();
      //auto s = std::string (v.begin (), v.end ());
      std::cout
        << input
        << " : "
        << v
        << std::endl
        ;
    }
    else
    {
      std::cout << r.message << std::endl;
    }
  }


  void test_json ()
  {
    //parse_and_print (R"("Hello")");
    //parse_and_print (R"("Hello\r\n\tThere")");
    parse_and_print ("1.0g32");
    parse_and_print ("-2");
    parse_and_print ("2.171828");
    parse_and_print ("1.32E3");
    parse_and_print ("2e-2");
    parse_and_print ("3.1415e10");
  };
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
int main()
{
  std::cout << "Running tests..." << std::endl;
  test_parser::test_opt<std::string> ("1234", "5678");
  test_parser::test_opt<int> (1,3);
  test_parser::test_parser ();
  json::test_json ();
  calculator::test_calculator ();
  std::cout << "Done!" << std::endl;
  return 0;
}
// ----------------------------------------------------------------------------
