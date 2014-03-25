USE [UniversityCodeFirstDatabase]
GO
SELECT 
[dbo].[Disciplines_Groups].[Discipline_Id], 
[dbo].[Disciplines_Groups].[Group_Id], 
[dbo].[Disciplines].[Title], 
[dbo].[Groups].[Acronym]

FROM [dbo].[Disciplines_Groups]

JOIN [dbo].[Disciplines] ON [dbo].[Disciplines_Groups].[Discipline_Id] = [dbo].[Disciplines].[Id]
JOIN [dbo].[Groups] ON [dbo].[Disciplines_Groups].[Group_Id] = [dbo].[Groups].[Id]

WHERE [dbo].[Disciplines].[Id] IN (1,2) -- AND [dbo].[Groups].[Id] > 5
ORDER BY [dbo].[Groups].[Id] DESC
GO