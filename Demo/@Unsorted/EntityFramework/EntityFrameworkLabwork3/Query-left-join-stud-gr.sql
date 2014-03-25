USE [UniversityCodeFirstDatabase]
SELECT 
	[dbo].[Students].[Name] + ' ' + 
		[dbo].[Students].[Surname] 
			AS StudentFullName,
	[dbo].[Groups].[Acronym] + '-' + 
		CAST([dbo].[Groups].[Course] AS NVARCHAR) + 
		CAST([dbo].[Groups].[DepartmentIndex] AS NVARCHAR) 
			AS GroupTitle
    FROM [dbo].[Students] 
    JOIN [dbo].[Groups] 
	ON [dbo].[Students].[Group_Id] = [dbo].[Groups].[Id]