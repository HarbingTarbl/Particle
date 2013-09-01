#ifndef _BINDING_H_
#define _BINDING_H_
#include <stack>
#include <boost\noncopyable.hpp>

namespace particle_
{
	using namespace std;

	template<typename T>
	class Binding
		: boost::noncopyable
	{
	private:
		static stack<T> m_stack;

	protected:
		typename T m_binding;
		static typename T m_top;

	public:
		inline void Reset (const T& data)
		{
			m_binding = data;
			m_binding.Bind();
		}

		inline Binding<T>(const T& val)
			: m_binding(val)
		{
			m_stack.push(m_top = m_binding);
			m_top->Bind();
		}

		inline ~Binding()
		{
			m_stack.pop();
			if(!m_stack.empty())
				(m_top = m_stack.top())->Bind();
		}

		inline bool IsBound() const  { return m_top == m_binding; }
	};
};
#endif