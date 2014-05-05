
__forceinline InteractiveTV::Project::Oasis *InteractiveTV::Project::Oasis::GetForCurrentThread()
{
	static Oasis s_oasis; return &s_oasis;
}