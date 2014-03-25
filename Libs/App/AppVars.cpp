#include "AppVars.h"

void Nena::App::Vars::EraseRecord(String const &key)
{
	AcquireSRWLockExclusive(&RecsLock);
	Recs.unsafe_erase(key);
	ReleaseSRWLockExclusive(&RecsLock);
}

Nena::App::Vars::RecordsSizeType Nena::App::Vars::GetRecordsCount()
{
	return Recs.size();
}

Nena::App::Vars::HResultSync Nena::App::Vars::Read(BOOL clearRecs, String file)
{
	std::ifstream in(file);
	if (in.bad()) return Flush(clearRecs, file);

	AcquireSRWLockExclusive(&RecsLock);

	std::stringstream buffer;
	buffer << in.rdbuf();
	std::string content(buffer.str());
	buffer.clear();
	in.close();

	Doc storage;
	storage.clear();
	storage.parse<0>((LPSTR) content.data());

	if (clearRecs) Recs.clear();
	Node *appNode = storage.first_node("App");
	Node *recNode = appNode->first_node("Vars");
	recNode = recNode->first_node("Rec");
	for (; recNode; recNode = recNode->next_sibling("Rec"))
		Recs[recNode->first_attribute("Name")->value()] = recNode->value();

	ReleaseSRWLockExclusive(&RecsLock);

	return S_OK;
}

Nena::App::Vars::HResultSync Nena::App::Vars::Flush(BOOL clearRecs, String file)
{
	AcquireSRWLockShared(&RecsLock);

	std::string content;

	Doc storage;

	Node *recNode = nullptr;
	Node *appNode = storage.allocate_node(rapidxml::node_element, "App");
	Node *varsNode = storage.allocate_node(rapidxml::node_element, "Vars");
	for (auto pair : Recs)
	{
		recNode = storage.allocate_node(rapidxml::node_element, "Rec");
		varsNode->append_node(recNode);

		recNode->value(storage.allocate_string(pair.second.c_str()));
		recNode->append_attribute(storage.allocate_attribute("Name",
			storage.allocate_string(pair.first.c_str()))
			);
	}

	appNode->append_node(varsNode);
	storage.append_node(appNode);

	rapidxml::print(
		std::back_inserter(content),
		storage
		);

	std::ofstream out(file);
	out << content;
	out.close();
	content.clear();
	storage.clear();

	if (clearRecs) Recs.clear();
	ReleaseSRWLockShared(&RecsLock);

	return S_OK;
}

Nena::App::Vars::HResultAsync Nena::App::Vars::ReadAsync(BOOL clearRecs, String file )
{
	return concurrency::create_task([this, clearRecs, file] { return Read(clearRecs, file); });
}

Nena::App::Vars::HResultAsync Nena::App::Vars::FlushAsync(BOOL clearRecs, String file)
{
	return concurrency::create_task([this, clearRecs, file] { return Flush(clearRecs, file); });
}

Nena::App::Vars::Vars() : Recs(), RecsLock()
{
	InitializeSRWLock(&RecsLock);
}

Nena::App::Vars::~Vars()
{
	Recs.clear();
}

Nena::App::Vars *Nena::App::Vars::GetForCurrentThread() 
{ 
	static Vars appVars; 
	return &appVars;
}

Nena::App::Vars::HResultSync Nena::App::Vars::HasRecord(String const &key) const
{
	return Recs.end() != Recs.find(key);
}

Nena::App::Vars::HResultAsync Nena::App::Vars::HasRecordAsync(String key) const
{
	return concurrency::create_task([this, key] { return HasRecord(key); });
}


