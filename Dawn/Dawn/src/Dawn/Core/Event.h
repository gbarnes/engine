#pragma once
#include "inc_common.h"
#include <map>
#include <vector>
#include <functional>

#define BIND_EVENT_MEMBER(x) std::bind(&x, this, std::placeholders::_1)
#define BIND_EVENT_STATIC(x) std::bind(&x)
#define EVENT_KEY(x) std::string(x)

namespace Dawn
{
	class DAWN_API Event
	{
	public:
		virtual std::string ToString() const { return std::string(""); }

	};



	typedef std::function<void(Event&)> EventDelegate;

	struct DAWN_API SEventHandle
	{
		u32 Index;
		std::string Key;
	};

	struct DAWN_API SEventDelegateData
	{
		EventDelegate Delegate;
		SEventHandle Handle;
	};


	typedef std::map<std::string, std::vector<SEventDelegateData>> EventSubscriberList;
	typedef std::pair<std::string, std::vector<SEventDelegateData>> EventSubscribePair;

	class DAWN_API CEventDispatcher
	{
	public:

		//
		//
		//
		static SEventHandle Subscribe(std::string& InKey, EventDelegate InDelegate)
		{
			// TODO: change this to return some sort of delegate handle 
			// to be able to remove it again!
			EventSubscriberList::iterator it = Subscribers.find(InKey);

			SEventDelegateData Data;
			Data.Delegate = InDelegate;
			Data.Handle.Key = InKey;
			Data.Handle.Index = 0;

			if (it == Subscribers.end()) 
			{
				std::vector<SEventDelegateData> list;
				list.push_back(Data);
				Subscribers.insert(EventSubscribePair(InKey, list));
			}
			else
			{
				// TODO: might pointers be more optimal here?!
				Data.Handle.Index = (u32)(Subscribers[InKey].size() - 1);
				Subscribers[InKey].push_back(Data);
			}

			return Data.Handle;
		}

		//static void Unsubscribe(std::string& InKey, )

		//
		//
		//
		static void Trigger(std::string& InKey, Event& InEvent)
		{
			EventSubscriberList::iterator it = Subscribers.find(InKey);
			if (Subscribers.find(InKey) == Subscribers.end())
				return;

			std::vector<SEventDelegateData> list = it->second;
			for (int i = 0; i < list.size(); ++i)
			{
				list[i].Delegate(InEvent);
			}
		}
	private:
		static EventSubscriberList Subscribers;
	};
};