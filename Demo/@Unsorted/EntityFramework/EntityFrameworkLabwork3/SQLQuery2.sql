USE [UniversityCodeFirstDatabase]

SELECT [dbo].[Groups].[Acronym] AS GroupAcro, COUNT(*) AS Total --,
--[dbo].[Students].[Surname] + ' ' + [dbo].[Students].[Name] AS StudentsName
--[dbo].[Groups].[Acronym]

FROM [dbo].[Students]
JOIN [dbo].[Groups] ON [dbo].[Students].[Group_Id] = [dbo].[Groups].[Id]
--WHERE [dbo].[Students].[Id] > 10

GROUP BY [dbo].[Groups].[Acronym]
HAVING COUNT(*) > 2

ORDER BY [dbo].[Groups].[Acronym] DESC
